# 火车票管理系统 开发文档


### 分工一览


| 主体逻辑 | B+树 |
| -------- | -------- |
| @bbty  | @z_taylcr7  |

## 程序功能概述

本程序将尝试实现类似于12306的售票系统。

## 主体逻辑说明

###### 主体结构：输入--解释输入-执行函数（-抛出异常--回到输入）--存储&更新数据--回到输入




## 代码文件结构


##### 外存文件

- **log_data.dat**

- **log_index.dat's inners**

- **log_index.dat's leaves**

- **train_data.dat**

- **mtrain_index.dat's inners**

- **mtrain_index.dat's leaves**

- **strain_index.dat's inners**

- **strain_index.dat's leaves**

- **order_data.dat**

- **order_index.dat's inners**

- **order_index.dat's leaves**

- **seat_data.dat**

- **seat_index.dat's inners**

- **seat_index.dat's leaves**

- **user_data.dat**

- **user_index.dat's inners**

- **user_index.dat's leaves**

以上文件分别有"_rb"后缀的rollback备份

##### 头文件

- **base.h**：一些基础的class
- **BPlusTree.hpp：**B+树数据结构
- **cmd.hpp**：解释输入输出
- **LRUMemoryPool.hpp**：文件读写
- **STL.h**：封装STL的vector,string,hashmap,sort
- **String.hpp**:文件读写中用到的char实现string
- **System.h**：火车票系统
- **Train.h**：火车类的实现
- **TrainManager.h**：处理有关火车的所有操作
- **User.h**：用户类的实现
- **UserManager.h**：处理有关火车的所有操作
- **utility.hpp**：b+树调用的函数（lower_bound&upper_bound）

### 维护以下的 B+ 树（外存）：B+树：\<key> -> data

1.**user_index** 所有用户信息：<username,pos> -> pos(user)

2.**strain_index **每辆火车信息：<trainID,pos>->pos(train)

3.**mtrain_index **到某站的所有火车：<station_name,trainID> ->pos(train)

4.**order_index **所有队列中等待的订单信息：<trainID,order_number> ->pos(order)

5.**seat_index **所有座位信息：<trainID+date,pos>->pos(seat)

6.**log_index **所有用户的订单信息：<username,num> ->pos(log)

## 各个类的接口及成员说明

用户类(User.h)

```cpp
class User{
private:
	_string username,password,name,mailAddr;
	int privilege;
	int ticketNum=0;
public:
	string ID_message()
	bool check_pass(string _password)
	int&priority()
	int query_ticketNum ()
    //以下为了modify_profile
	_string&user()
	_string&pass()
	_string&nam()
	_string&mail()
	void add_ticket()
};
```

火车类（Train.h）

```cpp
class Train{
private:
	class Station{
	public:
		RealTime arrival,departure;
		_string name;
		int price;
	};
	Station station[102];// 记录第一班车的到离时间 
	_string trainID;
	int stationNum;
	int seatNum;
	Time startTime;
	Date sale_st,sale_ed;
	bool if_release;
	char train_type;
	
public:
	bool if_released()
	void release()
	string train_id()
	pair<Date,Date>date()
	int station_num()
	string station_name
	void stations(vector<string>&o)//输出所有站名 
	void query_train(vector<string>&o,const Date&date,RemainedSeat seat=RemainedSeat())// 输出某天始发的列车信息 
	string info(const string&st_station,const string&ed_station,const Date&d,const RemainedSeat&seat)// 查询票价和座位数 
	bool check_date(const Date&d,const string&station_name)// 检查第d天到某站是否有车次 
	long long get_price(const pair<int,int>&o,const int n=1)
	int get_time(const pair<int,int>&o)//两站间花费的时间
	int seat_num()
};
```

座位信息类(Train.h)
```cpp
class RemainedSeat{//每一班车的剩余座位信息 
private:
	_string trainI_Date;
	int seat_remained[102];//每站剩余座位数量
public:
	int min_seat(const int&st,const int&ed)//做少座位数
	void de_seat(const int&st,const int&ed,const int&n)//减少座位数（买票成功后）
};

```

用户的订单类(Train.h)

```cpp
class Log{//订单 
private:
	int Pos;//这是该用户的第几单
	Status status;
	_string username,trainID,Fr,To;
	RealTime arrival,departure;
	int price,num;
public:
	string show_message()const{
		return "["+status_string()+"] "+(string)trainID+" "+(string)Fr+" "+departure.show_message()+" -> "+(string)To+" "+arrival.show_message()+" "+std::to_string(price)+" "+std::to_string(num);
	}
	void modify_status(const Status&_status)
	int serial_number()
	Status Status_now()
	pair<string,string> stations()
	pair<RealTime,RealTime> times()
	int number()const{return num;}
};
```

排队的订单类（System.h）

```cpp
class Order{
	public:
		int serial_num;//先来后到 
		int Pos;
		_string username,trainID;
		Date date;
		_string depart,arrive;
		int num;
	};
```

火车票系统各接口实现（System.h）

```cpp
class System{
public:
	void printbacis(){user.printbacis();} 
	bool release_train(const string&trainID)
	bool logout(const string&username)
	bool login(const string&username,const string&password)
	bool delete_train(const string&trainID)
	void query_train(const string&trainID,Date date,vector<string>&o)
	bool add_user(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege)
	bool add_train(const string&_trainID,const int&n,const int&m,const vector<string>&station_name,vector<int>&_price,const Time&st_time,vector<int>&travelTimes,vector<int>&stopoverTimes,const Date&st,const Date&ed,const char&_train_type)
	void query_ticket(const string&station_s,const string&station_t,const Date&date,const bool&if_time,vector<string>&o)
	void query_transfer(const string&station_s,const string&station_t,const Date&date,const bool&if_time,vector<string>&o)
	void query_order(const string&username,vector<string>&o)
	string query_profile(const string&ouser,const string&username)
	string modify_profile(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege)
	void clean()
	int query_priority(const string&username)
	bool refund_ticket(const string&username,int n)
	string buy_ticket(const string&username,const string&trainID,const Date&date,const string&station_s,const string&station_t,int n,bool q)
	bool rollback(const int&ddl)
}
```



