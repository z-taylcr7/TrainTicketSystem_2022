#include"TrainManager.h"
#include"UserManager.h"

class System{
	class Order{
	public:
		int serial_num;
		int id;
		MyString username,trainID;
		Date date;
		MyString depart,arrive;
		int num;
		
		Order()=default;
		Order(const int&_serial_num,const int&_id,const string&_username,const string&_trainID,const Date&_date,const string&_depart,const string&_arrive,const int&_num):serial_num(_serial_num),id(_id),username(_username),trainID(_trainID),date(_date),depart(_depart),arrive(_arrive),num(_num){}
		bool operator<(const Order&o)const{return serial_num<o.serial_num;}
	};
	class OrderStorage{
	private:
struct ints {int value=0; ints()=default; explicit ints(int x):value(x){}};
		BPlusTree<> order_index;
		MemoryPool<Order,ints>*order_data;
	public:
		OrderStorage():order_index("order_index.dat"){
			order_data=new MemoryPool<Order,ints>("order_data.dat",ints(0),1000000/sizeof(Order));
		}
		~OrderStorage(){
			delete order_data;
		}
		int get_id(const string&trainID,const int&id){
			vector<pair<int,ll> >tmp;
			order_index.find(trainID,tmp);
			if(tmp.empty()) return -404;
			return tmp[0].first;
		}
		void add_order(const string&trainID,const int&num,const Order&order){
			order_data->writeExtraBlock((ints)num);//更新总订单数 
			int id=order_data->add(order);
			order_index.insert({trainID,num},id);
		}
		void update(const int&id,const Order&order){
			order_data->update(id,order);
		}
		Order get_order(const int&id){
			return order_data->get(id);
		}
		int order_num(){return order_data->readExtraBlock().value;}
		void remove_order(const int&id,const string&trainID,const int&num){
			order_index.remove({trainID,num},id);
			order_data->remove(id); 
		}
		void get_ids(const string&trainID,vector<pair<ll,int> >&o){
			vector<pair<int,ll> >tmp;
			order_index.find(trainID,tmp);
			for(int i=0;i<tmp.size();i++) o.push_back({tmp[i].second,tmp[i].first});
			sort(o.begin(),o.end());
		}
		void clean(){
			order_index.clear();
			order_data->clearAll();
		}
		void rollback(const int&ddl){
			order_index.rollback(ddl);
			order_data->rollback(ddl);
		}
	};
	TrainManager train;
	UserManager user;
	OrderStorage orders;
	void add_order(const int&_id,const string&_username,const string&_trainID,const Date&_date,const string&_depart,const string&_arrive,const int&_num){
		int num=orders.order_num()+1;
		Order new_order(num,_id,_username,_trainID,_date,_depart,_arrive,_num);
		orders.add_order(_trainID,num,new_order);
	}
	
public:
	System()=default;
	~System()=default;
	void printbacis(){user.printbacis();} 
	bool release_train(const string&trainID){
		return train.release_train(trainID);
	}
	bool logout(const string&username){
		return user.logout(username);
	}
	bool login(const string&username,const string&password){
		return user.login(username,password);
	}
	bool delete_train(const string&trainID){
		return train.delete_train(trainID);
	}
	void query_train(const string&trainID,Date date,vector<string>&o){
		train.query_train(trainID,date,o);
	}
	bool add_user(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege){
		return user.add_user(ouser,username,password,name,mailAddr,privilege);
	}
	bool add_train(const string&_trainID,const int&n,const int&m,const vector<string>&station_name,vector<int>&_price,const Time&st_time,vector<int>&travelTimes,vector<int>&stopoverTimes,const Date&st,const Date&ed,const char&_train_type){
		return train.add_train(_trainID,n,m,station_name,_price,st_time,travelTimes,stopoverTimes,st,ed,_train_type); 
	} 
	void query_ticket(const string&station_s,const string&station_t,const Date&date,const bool&if_time,vector<string>&o){
		train.query_ticket(station_s,station_t,date,if_time,o);
	} 
	void query_transfer(const string&station_s,const string&station_t,const Date&date,const bool&if_time,vector<string>&o){
		train.query_transfer(station_s,station_t,date,if_time,o);
	} 
	void query_order(const string&username,vector<string>&o){
		if(user.check_priority(username)<0) o.push_back("-1");
		else train.query_order(username,o);
	}
	string query_profile(const string&ouser,const string&username){
		return user.query_profile(ouser,username);
	}
	string modify_profile(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege){		
		return user.modify_profile(ouser,username,password,name,mailAddr,privilege);
	}
	void clean(){
		user.clean();
		train.clean();
	}
	int query_priority(const string&username){
		return user.query_priority(username);
	}
	bool refund_ticket(const string&username,int n){
		if(user.check_priority(username)<0) return 0;
		int tot_num=user.query_ticketNum(username);
		int num=tot_num+1-n;
		if(num<=0||num>tot_num) return 0;//订单不合法 
		
		Date date;
		pair<string,int> back=train.refund_ticket(username,num,date); 
		if(back.second==REFUNDED) return 0;//订单不存在
		
		ll hash_u=hash_int(username);
		vector<pair<ll,int> >order_index;
		orders.get_ids(back.first,order_index);
		//to be simplified
		if(back.second==PENDING){
			for(int i=0;i<order_index.size();i++){
				int id=order_index[i].second;
				Order order(orders.get_order(id));
				if(order.id==num&&hash_int((string)order.username)==hash_u){
					orders.remove_order(id,back.first,order.serial_num);
					break;
				}
			}
		}else{
			Train train1=train.get_train(back.first);
			int seat_id=train.get_seat_id(back.first,date);
			RemainedSeat seats=train.get_seat(seat_id);
			for(int i=0;i<order_index.size();i++){
				int order_id=order_index[i].second;
				Order order(orders.get_order(order_id));
				string departure(order.depart),arrival(order.arrive),user(order.username);
				Date d=train1.setoff_date(departure,order.date);
				if(!(d==date)) continue;
				if(train.re_buy_ticket(departure,arrival,order.num,order.id,user,train1,seats)){
					orders.remove_order(order_id,back.first,order.serial_num);
				}
			}
			train.update_seat(seat_id,seats);
		}
		return 1;
	}
	string buy_ticket(const string&username,const string&trainID,const Date&date,const string&station_s,const string&station_t,int n,bool q){
//		std::cout<<"buy ticket here"<<std::endl;
		if(user.check_priority(username)<0) return "-1";
		int num=user.query_ticketNum(username);
//		std::cout<<"!!!"<<std::endl;
		ll cost=train.buy_ticket(trainID,date,station_s,station_t,n,num+1,username,q);
//		std::cout<<"12345678"<<std::endl;
		if(cost>0){
			user.add_ticketNum(username);
			return std::to_string(cost);
		}else{
			if(cost<0) return "-1";
			if(q){
//				puts("!!!") ;
				add_order(num+1,username,trainID,date,station_s,station_t,n);
				user.add_ticketNum(username);
				return "queue"; 
			}else return "-1";
		}
	}
	bool rollback(const int&ddl){
//		std::cout<<"del train here"<<std::endl; 
		train.rollback(ddl);
		
//		std::cout<<"del user here"<<std::endl;
		user.rollback(ddl);
		
//		std::cout<<"del order here"<<std::endl;
		orders.rollback(ddl); 
		return 0;
	}
};
