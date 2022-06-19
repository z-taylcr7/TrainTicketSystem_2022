#include "base.h"

class RemainedSeat{//每一班车的剩余座位信息 
private:
	MyString trainI_Date;
	int seat_remained[102];
public:
	RemainedSeat()=default;
	RemainedSeat(const string&main_key):trainI_Date(main_key){}
	RemainedSeat(const string&main_key,const int&station_num,const int&seat_num):trainI_Date(main_key){
		for(int i=0;i<=station_num;i++) seat_remained[i]=seat_num;
	}
	
	int min_seat(const int&st,const int&ed)const{
		int res=seat_remained[st];
		for(int i=st;i<ed;i++) res=std::min(res,seat_remained[i]);
		return res;
	}
	void de_seat(const int&st,const int&ed,const int&n){
		for(int i=st;i<ed;i++) seat_remained[i]-=n;
	}
	int operator[](const int&index)const{return seat_remained[index];}	
};

class Train{
private:
	class Station{
	public:
		RealTime arrival,departure;
		MyString name;
		int price;
		
		Station()=default;
		void update(const string&_name,const int&_price,const RealTime&arr,const RealTime&dep){
			name=_name;
			price=_price;
			arrival=arr;
			departure=dep;
		}
		void update(const string&_name,const int&_price){
			name=_name;
			price=_price;
		}
	};
	
	Station station[102];// 记录第一班车的到离时间 
	MyString trainID;
	int stationNum;
	int seatNum;
	Time startTime;
	Date sale_st,sale_ed;
	bool if_release;
	char train_type;
	
public:
	pair<int,int> get_id(const string&st,const string&ed)const{
		auto hash_st=hash_int(st),hash_ed=hash_int(ed);
		int St=-404,Ed=-404;
		for(int i=0;i<stationNum;i++){
			auto hash_i=hash_int((string)station[i].name);
			if(hash_st==hash_i) St=i;
			if(hash_ed==hash_i){Ed=i;break;}
		}
		return {St,Ed};
	}
	int get_id(const string&_name)const{//是第几站 
		for(int i=0;i<stationNum;i++){
			if(station[i].name==_name) return i;
		}
		return stationNum;
	}
	
	Train()=default;
	~Train()=default;
	Train(const string&_trainID,const int&n,const int&m,const vector<string>&station_name,vector<int>&_price,const Time&st_time,vector<int>&travelTimes,vector<int>&stopoverTimes,const Date&st,const Date&ed,const char&_train_type):trainID(_trainID),startTime(st_time),sale_st(st),sale_ed(ed),stationNum(n),seatNum(m),train_type(_train_type){
//		if(_train_type>'Z'||_train_type<'A') std::cout<<trainID<<" "<<_train_type; 
		if_release=0;
		station[0].update(station_name[0],0);
		station[0].departure=RealTime(Date(6,1),st_time);
		for(int i=0;i<n-2;i++){
			RealTime arrival(station[i].departure+travelTimes[i]);
			station[i+1].update(station_name[i+1],station[i].price+_price[i],arrival,arrival+stopoverTimes[i]);
		}
		station[n-1].update(station_name[n-1],station[n-2].price+_price[n-2]);
		station[n-1].arrival=station[n-2].departure+travelTimes[n-2];
//		query_train()
//		std::cout<<_trainID<<"\n";
//		for(int i=1;i<stationNum;i++){
//			std::cout<<station[i].arrival.minute<<" ";
//		}
//		std::cout<<"\n";
//		for(int i=0;i<stationNum-1;i++){
//			std::cout<<station[i].departure.minute<<" ";
//		}
//		std::cout<<"\n";
	}
	bool operator<(const Train&o)const{return trainID<o.trainID;}
	bool if_released()const{return if_release;}
	void release(){if_release=1;}
	string train_id()const{return (string)trainID;}
	pair<Date,Date>date()const{return{sale_st,sale_ed};}
	int station_num()const{return stationNum;}
	string station_name(const int&pos)const{return (string)station[pos].name;}
	void stations(vector<string>&o)const{//输出所有站名 
		for(int i=0;i<stationNum;i++) o.push_back((string)station[i].name);
	}
	
	Date departure_date(const string&station_name,const Date&d)const{// 从第d天始发的列车离开某站的时间 
		int id=get_id(station_name);
		RealTime tmp=station[id].departure+RealTime(d);
		return tmp.date();
	}
	Time departure_time(const string&station_name)const{// 离开某站的时间 ，相对于始发时 
		int id=get_id(station_name);
		return station[id].departure.time();
	}
	Date setoff_date(const string&station_name,const Date&d)const{// 离开某站的绝对天数 
		int id=get_id(station_name);
		return d-station[id].departure.date().dayNum()+1;
	}
	void query_train(vector<string>&o,const Date&date,RemainedSeat seat=RemainedSeat())const{// 输出某天始发的列车信息 
		o.push_back((string)trainID+" "+train_type);
		for(int i=0;i<stationNum;i++){
			RealTime arrival(station[i].arrival+RealTime(date));
			RealTime departure(station[i].departure+RealTime(date));
			if(station[i].arrival.minute<0) arrival.minute=-404;
			if(station[i].departure.minute<0) departure.minute=-404;
			string tmp=(string)station[i].name+" "+arrival.show_message()+" -> "+departure.show_message()+" "+std::to_string(station[i].price)+" ";
			if(i==stationNum-1) tmp+="x";
			else{
				if(!if_release) tmp+=std::to_string(seatNum);
				else tmp+=std::to_string(seat[i]);
			}
			o.push_back(tmp);
		}
	}
	string info(const string&st_station,const string&ed_station,const Date&d,const RemainedSeat&seat)const{// 查询票价和座位数 
		bool if_include=0;
		int st=0,ed=0,seat_num=seatNum;
		for(int i=0;i<stationNum;i++){
			if(st_station==station[i].name){st=i;if_include=1;}
			if(ed_station==station[i].name){ed=i;break;}
			if(if_include) seat_num=std::min(seat_num,seat[i]);
		}
		int price=station[ed].price-station[st].price;
		int time=station[ed].arrival-station[st].departure;
//		std::cout<<st<<" "<<ed<<" "<<stationNum<<"\n";
//		std::cout<<"!!"<<time<<"!!\n";
//		for(int i=1;i<stationNum;i++){
//			std::cout<<station[i].arrival.minute<<" ";
//		}
//		std::cout<<"\n";
//		for(int i=0;i<stationNum-1;i++){
//			std::cout<<station[i].departure.minute<<" ";
//		}
//		std::cout<<"\n";
		RealTime departure(d,station[st].departure.time()),arrival(departure+time);
		return (string)trainID+" "+st_station+" "+departure.show_message()+" -> "+ed_station+" "+arrival.show_message()+" "+std::to_string(price)+" "+std::to_string(seat_num);
	}
	bool check_date(const Date&d,const string&station_name)const{// 检查第d天到某站是否有车次 
		Date tmp=setoff_date(station_name,d);
		if(tmp<sale_st||sale_ed<tmp) return 0;
		return 1;
	}
	bool check_sequence(const pair<int,int>&o)const{
		return o.first<o.second&&o.first>=0;
	}
	long long get_price(const pair<int,int>&o,const int n=1)const{ 
		return 1ll*(station[o.second].price-station[o.first].price)*n;
	}
	int get_time(const pair<int,int>&o)const{
		return station[o.second].arrival-station[o.first].departure;
	}
	pair<RealTime,RealTime> obtain_time(const pair<int,int>&o,const Date&d){
		return {station[o.first].departure+RealTime(d),station[o.second].arrival+RealTime(d)};
	}
	pair<int,Date> check_later(const Train train,const Date&st_date,const string&st_station,const string&trans_station)const{//从始发站到换乘站，查询最快换乘时间 
		auto id=get_id(st_station,trans_station);
		int date=station[id.second].arrival.date()-station[id.first].departure.date();
		Date arrival_date=st_date+date;
		Time arrival_time=station[id.second].arrival.time();
		RealTime arrival(arrival_date,arrival_time);
		
		Date last_date=train.departure_date(trans_station,train.sale_ed);
		Time d_time=train.departure_time(trans_station); 
		if(RealTime(last_date,d_time)<=arrival) return {-1,Date()};//换乘失败 
		//to be simplified
		Date first_date=train.departure_date(trans_station,train.sale_st);
		for(Date i=first_date;i<=last_date;++i){
			RealTime tmp(i,d_time);
			if(arrival<=tmp) return {tmp-arrival,i};//tmp-arrival为等待时间 ,i为登车时间 
		}
		puts("get sth wrong here:Train.h:class Train:check_later");
		return {0,Date()};
	}
	int seat_num()const{return seatNum;}
	int check_seat(const pair<int,int>&o,const RemainedSeat&seat)const{
		return seat.min_seat(o.first,o.second);
	}
	void de_seat(const pair<int,int>&o,const int&n,RemainedSeat&seat)const{
		seat.de_seat(o.first,o.second,n);
	}
	void in_seat(const pair<int,int>&o,const int&n,RemainedSeat&seat)const{
		seat.de_seat(o.first,o.second,-n);
	}
};

enum Status{SUCCESS=1,PENDING=0,REFUNDED=-1};
class Log{//订单 
private:
	int id;
	Status status;
	MyString username,trainID,Fr,To;
	RealTime arrival,departure;
	int price,num;
public:
	Log()=default;
	Log(const int&_id,const Status _status,const string&_username,const string&_trainID,const string&_Fr,const string&_To,const RealTime&_departure,const RealTime&_arrival,const int&_price,const int&_num):username(_username),trainID(_trainID),Fr(_Fr),To(_To),departure(_departure),arrival(_arrival),status(_status),price(_price),num(_num),id(_id){}
	string show_message()const{
		return "["+status_string()+"] "+(string)trainID+" "+(string)Fr+" "+departure.show_message()+" -> "+(string)To+" "+arrival.show_message()+" "+std::to_string(price)+" "+std::to_string(num);
	}
	void modify_status(const Status&_status){status=_status;}
	int serial_number()const{return id;}
	Status Status_now()const{return status;}
	string status_string()const{
		if(status==SUCCESS) return "success";
		if(status==PENDING) return "pending";
		return "refunded";
	}
	string train()const{return (string)trainID;}
	pair<string,string> stations()const{return {(string)Fr,(string)To};}
	pair<RealTime,RealTime> times()const{return {departure,arrival};}
	int number()const{return num;}
	bool operator<(const Log&o)const{return id<o.id;}
};












