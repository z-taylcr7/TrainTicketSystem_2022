#include"Train.h"

class TrainManager{
private:
	class TrainStorage{
	private:
		BPlusTree<> mtrain_index;
		BPlusTree<> strain_index;
		MemoryPool<Train,bool>*train_data;
	public:
		TrainStorage():mtrain_index("mtrain_index.dat"),strain_index("strain_index.dat"){
			train_data= new MemoryPool<Train,bool>("train_data.dat",0,1000000/sizeof(Train));
		}
		~TrainStorage(){
			delete train_data;
		}
		int get_id(const string&train_id){// �ҵ�train_id�Ĵ����ַ 
			vector<pair<int,ll> >tmp;
//			std::cout<<"<wtf>";
			strain_index.find((train_id),tmp);
//			std::cout<<"<wtf>";
			if(tmp.empty()) return -404;
			return tmp[0].first;
		}
		int get_ids(const string&station_name,vector<pair<int,ll> >&o){//�ҵ�;��station_name�����л� 
			mtrain_index.find(station_name,o);
//			std::cout<<"get_ids of "<<station_name<<"  "<<o.size()<<"\n";
			if(o.empty()) return -404;
			return o.size();
		}
		Train get_train(const int&id){return train_data->get(id);}
		void get_trains(const string&station_name,vector<Train>&o){
			vector<pair<int,ll> >tmp;
			mtrain_index.find(station_name,tmp);
			for(int i=0;i<tmp.size();i++) o.push_back(train_data->get(tmp[i].first));
		}
		void update(const int&id,const Train&o){
			train_data->update(id,o);
		}
		void release(const int&id,const Train&train){
			
			vector<pair<int,ll> >tmp;
			
			ll key=hash_int(train.train_id());
			for(int i=0;i<train.station_num();i++) mtrain_index.insert({train.station_name(i),key},id);
			train_data->update(id,train);
		}
		void add_train(const string&train_id,const Train&train){
			int id=train_data->add(train);
			strain_index.insert({train_id,id},id);
		}
		void delete_train(const string&train_id,const int&id){
			strain_index.remove({train_id,id},id);
			train_data->remove(id);
		}
		void clean(){
			train_data->clearAll();
			strain_index.clear();
			mtrain_index.clear();
		}
		void rollback(const int&ddl){
			
//		std::cout<<"gg1"<<std::endl;
			mtrain_index.rollback(ddl);
//		std::cout<<"gg2"<<std::endl;
			strain_index.rollback(ddl);
//		std::cout<<"gg3"<<std::endl;
			train_data->rollback(ddl);
		}
	};
	class SeatStorage{
	private:
		BPlusTree<> seat_index;
		MemoryPool<RemainedSeat,bool>*seat_data;
		string get_key(const string&train_id,const Date&data)const{
			return train_id+data.show_message();
		}
	public:
		SeatStorage():seat_index("seat_index.dat"){
			seat_data=new MemoryPool<RemainedSeat,bool>("seat_data.dat",0,1000000/sizeof(RemainedSeat));
		}
		~SeatStorage(){
			delete seat_data;
		}
		int get_id(const string&train_id,const Date&date){
			vector<pair<int,ll> >tmp;
			seat_index.find(get_key(train_id,date),tmp);
			if(tmp.empty()) return -404;
//			if(tmp.size()>1) std::cout<<" wtf ";
			return tmp[0].second;
		}
		RemainedSeat get_seats(const int&id){
			return seat_data->get(id);
		}
		void release(const Train&train){
			string train_id=train.train_id();
			pair<Date,Date> pos=train.date();
			for(Date date=pos.first;date<=pos.second;++date){
				string main_key=get_key(train_id,date);
				RemainedSeat seat(main_key,train.station_num(),train.seat_num());
				int id=seat_data->add(seat);
				seat_index.insert({main_key,id},id);
			}
		}
		void update(const int&id,const RemainedSeat&seat){
			seat_data->update(id,seat);
		}
		void clean(){
			seat_index.clear();
			seat_data->clearAll();
		}
		void rollback(const int&ddl){
			seat_index.rollback(ddl);
			seat_data->rollback(ddl);
		}
	};
	class LogStorage{
	private:
		BPlusTree<>  log_index;
		MemoryPool<Log,bool>*log_data;
	public:
		LogStorage():log_index("log_index.dat"){
			log_data=new MemoryPool<Log,bool>("log_data.dat",0,1000000/sizeof(Log));
		}
		~LogStorage(){
			delete log_data;
		}
		int get_id(const string&username,const int&id){
			vector<pair<int,ll> >tmp;
			log_index.find(username,tmp);
			if(tmp.empty()) return -404;
			for(int i=0;i<tmp.size();i++)
				if(tmp[i].second==id) return tmp[i].first;
			return -404;
		}
		void add_log(const string&username,const int&id,const Log&log){
//		std::cout<<"add_log here"<<std::endl; 
			int pos=log_data->add(log);
//		std::cout<<"insert "<<username<<" "<<id<<std::endl; 
			log_index.insert({username,id},pos);
//		std::cout<<"finished"<<std::endl; 
		}
		void update(const int&log_id,const Log&log){
			log_data->update(log_id,log);
		}
		Log get_log(const int&log_id){return log_data->get(log_id);}
		
		void get_logs(const string&username,vector<Log>&o){
			vector<pair<int,ll> >tmp;
			log_index.find(username,tmp);
			for(int i=0;i<tmp.size();i++){
				o.push_back(log_data->get(tmp[i].first));
			}
			sort(o.begin(),o.end()); 
		}
		void clean(){
			log_index.clear();
			log_data->clearAll();
		}
		void rollback(const int&ddl){
			log_index.rollback(ddl);
			log_data->rollback(ddl);
		}
	};
	TrainStorage trains;
	SeatStorage seats;
	LogStorage logs;
	bool update_log(const string&username,const int&id,const Status&status){
		int log_id=logs.get_id(username,id);
		if(log_id<0) return 0;//δ�ҵ�����
		Log log(logs.get_log(log_id));
		log.modify_status(status);
		logs.update(log_id,log);
		return 1; 
	}
 	void write_log(const int&_id,const Status _status,const string&_username,const string&_trainID,const string&_Fr,const string&_To,const RealTime&_departure,const RealTime&_arrival,const int&_price,const int&_num){
 		
//		std::cout<<"write_log here"<<std::endl; 
		Log new_log(_id,_status,_username,_trainID,_Fr,_To,_departure,_arrival,_price,_num); 
		logs.add_log(_username,_id,new_log);
	}
public:
	TrainManager()=default;
	~TrainManager()=default;
	
	bool release_train(const string&trainID){
		int id=trains.get_id(trainID);
		if(id<0){
//            std::cout<<"zhaobudao"<<std::endl;
            return 0;//�г�������
        }
		Train  train(trains.get_train(id));
		if(train.if_released()){
//            std::cout<<"yijinrlsl"<<std::endl;
            return 0;//�г��ѷ���
        }
		train.release();
        trains.update(id,train);
		trains.release(id,train);
		seats.release(train); 
		return 1;
	}
	void clean(){
		trains.clean();
		seats.clean();
		logs.clean();
	}
	
	bool add_train(const string&_trainID,const int&n,const int&m,const vector<string>&station_name,vector<int>&_price,const Time&st_time,vector<int>&travelTimes,vector<int>&stopoverTimes,const Date&st,const Date&ed,const char&_train_type){
		int id=trains.get_id(_trainID);
		if(id>=0) return 0;//�г��Ѵ���
//		std::cout<<"<!!!>";
		Train new_train(_trainID,n,m,station_name,_price,st_time,travelTimes,stopoverTimes,st,ed,_train_type);
//		std::cout<<"<HERE>"; 
		trains.add_train(_trainID,new_train);
		return 1;
	}
	bool delete_train(const string&_trainID){
		int id=trains.get_id(_trainID);
		if(id<0) return 0;//�г�������
		Train train(trains.get_train(id));
		if(train.if_released()) return 0;//�г��ѷ���
		trains.delete_train(_trainID,id);
		return 1; 
	}
	void query_train(const string&trainID,Date date,vector<string>&o){
		int id=trains.get_id(trainID);
		if(id<0){//�г������� 
			o.push_back("-1");
			return;
		}
		Train train(trains.get_train(id));
		pair<Date,Date> day=train.date();
		if(date<day.first||day.second<date){//��ʱ���û�� 
			o.push_back("-1");
			return;
		}
		if(train.if_released()){
			int seat_id=seats.get_id(trainID,date);
			RemainedSeat seat(seats.get_seats(seat_id));
			train.query_train(o,date,seat);
		}else train.query_train(o,date);
	}
	void query_ticket(const string&station_s,const string&station_t,const Date&date,const bool&if_time,vector<string>&o){
		if(!date.is_fair()){
			o.push_back("0");
			return;
		}
		static vector<pair<int,ll> >trains1,trains2;
		trains1.clear();trains2.clear();
		trains.get_ids(station_s,trains1);
		trains.get_ids(station_t,trains2);
//		HashMap<ll,int>station;
		vector<pair<pair<int,string>,int> >list;
//		for(int i=0;i<trains1.size();i++) station.insert(trains1[i].second,1);
//		std::cout<<"------------------------------------------------------\n";
//		for(int i=0;i<trains2.size();i++) std::cout<<trains2[i].first<<" ";std::cout<<"\n"; 
//		for(int i=0;i<trains2.size();i++) std::cout<<trains2[i].second<<"\n"; std::cout<<"\n";
		for(int i=0,j=0;i<trains2.size()&&j<trains1.size();){
			if(trains2[i].second<trains1[j].second) i++;
			else if (trains2[i].second>trains1[j].second) j++;
			else{
				Train train(trains.get_train(trains2[i].first));
				pair<int,int>pos=train.get_id(station_s,station_t);
				if(train.check_date(date,station_s)&&train.if_released()&&train.check_sequence(pos)){
					if(if_time) list.push_back({{train.get_time(pos),train.train_id()},i});
					else list.push_back({{train.get_price(pos),train.train_id()},i});
				}
				i++,j++;
			}
		}
		if(list.empty()){
			o.push_back("0");
			return;
		}
		sort(list.begin(),list.end());
		o.push_back(std::to_string(list.size()));
		for(int i=0;i<list.size();i++){
			int id=trains2[list[i].second].first;
			Train train(trains.get_train(id));
			Date d=train.setoff_date(station_s,date);
			int seat_id=seats.get_id(train.train_id(),d);
			RemainedSeat seat(seats.get_seats(seat_id));
			o.push_back(train.info(station_s,station_t,date,seat));
		}
	}
	void query_order(const string&username,vector<string>&o){
		vector<Log>tmp;
		logs.get_logs(username,tmp);
		o.push_back(std::to_string(tmp.size()));
		for(int i=tmp.size()-1;i>=0;i--) o.push_back(tmp[i].show_message());
	}
	void query_transfer(const string&station_s,const string&station_t,const Date&date,const bool&if_time,vector<string>&o){
		if(!date.is_fair()){
			o.push_back("0");
			return;
		}
		static vector<pair<int,ll> >trains1,trains2;
		trains1.clear();trains2.clear();
		trains.get_ids(station_s,trains1);
		trains.get_ids(station_t,trains2);
		
		bool if_find=0;
		int cost1_min=-1,train1_id,train2_id,cost2_min=-1;
		string transfer_station;
		Date trans_date;
		ll hash_s=hash_int(station_s),hash_t=hash_int(station_t);
		string trainID1,trainID2;
		
		for(int i=0;i<trains1.size();i++){
			Train train1(trains.get_train(trains1[i].first));
			if(!train1.if_released()||!train1.check_date(date,station_s)) continue;
			ll hash_t1=hash_int(train1.train_id());
			HashMap<ll,int>stations;
			bool start_add=0;
			for(int j=0;j<train1.station_num();j++){
				ll hash_name=hash_int(train1.station_name(j));
				if(start_add) stations.insert(hash_name,j);
				if(hash_name==hash_s) start_add=1;
			}
			for(int j=0;j<trains2.size();j++){
				Train train2(trains.get_train(trains2[j].first));
				ll hash_t2=hash_int(train2.train_id());
				if(!train2.if_released()||hash_t1==hash_t2) continue;//ע���������г�һ����
				for(int k=0;k<train2.station_num();k++){
					const string&st=train2.station_name(k);
					ll hash_st=hash_int(st);
					if(hash_st==hash_t) break;
					if(stations.count(hash_st)!=0){
						pair<int,Date> tmp=train1.check_later(train2,date,station_s,st);
						if(tmp.first<0) continue;
						pair<int,int>pos1=train1.get_id(station_s,st),pos2=train2.get_id(st,station_t);
						int cost1=train1.get_price(pos1)+train2.get_price(pos2),cost2=train1.get_time(pos1)+tmp.first+train2.get_time(pos2);
						
						if(if_time) std::swap(cost1,cost2);
						if(!if_find||cost1<cost1_min||(cost1==cost1_min&&cost2<cost2_min)||((cost1==cost1_min&&cost2==cost2_min)&&(train1.train_id()<trainID1||(train1.train_id()==trainID1&&train2.train_id()<trainID2)))){
							if_find=1;
							cost1_min=cost1,cost2_min=cost2;
							train1_id=i;train2_id=j;
							transfer_station=st;
							trans_date=tmp.second;
							trainID1=train1.train_id();
							trainID2=train2.train_id();
						}
					}
				}
			}
		}
		if(!if_find){
			o.push_back("0");
			return;
		}
		Train train1(trains.get_train(trains1[train1_id].first));
		Date date1=train1.setoff_date(station_s,date);
		int seat_id1=seats.get_id(train1.train_id(),date1);
		RemainedSeat seat1(seats.get_seats(seat_id1));
		o.push_back(train1.info(station_s,transfer_station,date,seat1));
		
		Train train2(trains.get_train(trains2[train2_id].first));
		Date date2=train2.setoff_date(transfer_station,trans_date);
		int seat_id2=seats.get_id(train2.train_id(),date2);
		RemainedSeat seat2(seats.get_seats(seat_id2));
		o.push_back(train2.info(transfer_station,station_t,trans_date,seat2));
	}
	
	ll buy_ticket(const string&trainID,const Date&date,const string&station_s,const string&station_t,const int&n,const int&id,const string&username,bool q){
		int train_id=trains.get_id(trainID);
		if(train_id<0) return -404;
		Train train(trains.get_train(train_id));
		pair<int,int>pos=train.get_id(station_s,station_t);
		if(!train.if_released()) return -404;//�г�δ���� 
		if(!train.check_date(date,station_s)) return -404;//�޳��ΰ��� 
		if(!train.check_sequence(pos)) return -404;//��������վ
		if(train.seat_num()<n) return -404;
		Date d=train.setoff_date(station_s,date);
		int seat_id=seats.get_id(trainID,d);
		RemainedSeat seat(seats.get_seats(seat_id));
		int seat_remain=train.check_seat(pos,seat);
		ll tot_price=train.get_price(pos,n);
		pair<RealTime,RealTime> time=train.obtain_time(pos,d); 
		if(seat_remain>=n){
			train.de_seat(pos,n,seat);
			seats.update(seat_id,seat);
//		return 0;
			write_log(id,SUCCESS,username,trainID,station_s,station_t,time.first,time.second,tot_price/n,n);
			return tot_price;
		}else{
			if(!q) return -404;
//		return 0;
			write_log(id,PENDING,username,trainID,station_s,station_t,time.first,time.second,tot_price/n,n);
			return 0;
		}
	}
	pair<string,int>refund_ticket(const string&username,const int&n,Date&date){
		pair<string,int>o("fail",-1);
		int log_id=logs.get_id(username,n);
		if(log_id<0) return o;
		Log log(logs.get_log(log_id));
		if(log.Status_now()==REFUNDED) return o;
		o={log.train(),log.Status_now()};
		log.modify_status(REFUNDED);
		logs.update(log_id,log);
		if(o.second==PENDING) return o;
		int id=trains.get_id(o.first);
		Train train(trains.get_train(id));
		pair<string,string>stations=log.stations();
		pair<int,int>station_id=train.get_id(stations.first,stations.second);
		Date day=log.times().first.date();
		date=train.setoff_date(stations.first,day);
		int seat_id=seats.get_id(train.train_id(),date);
		RemainedSeat seat(seats.get_seats(seat_id));
		train.in_seat(station_id,log.number(),seat);
		seats.update(seat_id,seat);
		return o;
	}
	bool re_buy_ticket(const string&station_s,const string&station_t,const int&n,const int&id,const string&username,const Train&train,RemainedSeat&seat){
		pair<int,int>ids=train.get_id(station_s,station_t);
		int seat_remain=seat.min_seat(ids.first,ids.second);
		if(seat_remain>=n){
			seat.de_seat(ids.first,ids.second,n);
			update_log(username,id,SUCCESS);
			return 1;
		}else return 0;
	}
	Train get_train(const string&trainID){
		return trains.get_train(trains.get_id(trainID));
	}
	RemainedSeat get_seat(const int&id){
		return seats.get_seats(id);
	}
	void update_seat(const int&id,const RemainedSeat&seat){
		seats.update(id,seat);
	}
	int get_seat_id(const string trainID,const Date&date){
		return seats.get_id(trainID,date);
	}
	void rollback(const int&ddl){
		
//		std::cout<<"g1"<<std::endl;
		trains.rollback(ddl);
//		std::cout<<"g2"<<std::endl;
		seats.rollback(ddl);
//		std::cout<<"g3"<<std::endl;
		logs.rollback(ddl);
	}
};












