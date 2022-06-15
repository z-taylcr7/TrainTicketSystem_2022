
#ifndef nnnd
#define nnnd

#include "BPlusTree.hpp"
#include<string>
#include <iostream>
using std::ostream;
typedef long long ll;  


constexpr static int days[5]={0,30,61,92,122};

inline long long to_int(const string&A){
	long long ans=0;
	for(int i=0;i<A.size();i++) ans=ans*10+A[i]-'0';
	return ans;
}
inline long long hash_int(const string&A){
	long long ans=0;
	for(int i=0;i<A.size();i++) ans=(ans<<16)+ans+A[i];
	return ans;
}

class Time{
public:
	int hour=0,minute=0;
	
	Time()=default;
	~Time()=default;
	Time(int h,int m):hour(h),minute(m){}
	Time(const Time&o):hour(o.hour),minute(o.minute){}
	Time(const string&o):hour(to_int(o.substr(0,2))),minute(to_int(o.substr(3,2))){}
	
	Time&operator+=(int m){
		minute+=m;
		if(minute>=60){
			hour+=minute/60;
			minute%=60;
		}
		hour%=24;
		return *this;
	}
	Time operator+(int m)const{
		Time tmp(*this);
		tmp+=m;
		return tmp;
	}
    Time &operator=(const Time &t){
        if(this==&t) return *this;
        hour=t.hour;
        minute=t.minute;
        return *this;
    }
	string show_message()const{
		string h=std::to_string(hour),m=std::to_string(minute);
		if(h.size()==1) h="0"+h;
		if(m.size()==1) m="0"+m;
		return h+":"+m;
	}
	bool operator<(const Time&t)const{return hour<t.hour||(hour==t.hour&&minute<t.minute);}	
};

class Date{
public:
	int month=6,day=1;
	
	Date()=default;
	~Date()=default;
	Date(int m,int d):month(m),day(d){}
	Date(const Date&o):month(o.month),day(o.day){}
	Date(const string&o){
		month=to_int(o.substr(0,2));
		day=to_int(o.substr(3,2));
	}
	bool is_fair()const{return month>=6&&month<=8;}
	string show_message()const{
		string m=std::to_string(month),d=std::to_string(day);
		if(m.size()==1) m="0"+m;
		if(d.size()==1) d="0"+d;
		return m+"-"+d;
	}
	int dayNum()const{return days[month-6]+day;}
	bool operator<(const Date&o)const{return month<o.month||(month==o.month&&day<o.day);}
	bool operator==(const Date&o)const{return month==o.month&&day==o.day;}
	bool operator<=(const Date&o)const{return *this<o||*this==o;}
	int operator-(const Date&o)const{return dayNum()-o.dayNum();}
	Date operator-(const int&d)const{
		int tmp=dayNum()-d,mon=6;
		while(days[mon-5]<tmp) mon++;
		return Date(mon,tmp-days[mon-6]);
	}
	Date&operator++(){
		day++;
		if(dayNum()>days[month-5]){
			day=dayNum()-days[month-5];
			month++;
		}
		return *this;
	}
	Date operator+(const int&o)const{
		Date tmp=*this;
//		tmp.day+=o;
//		while(tmp.dayNum()>days[tmp.month-5]){
//			tmp.day=tmp.dayNum()-days[tmp.month-5];
//			tmp.month++;
//		}
		return tmp-(-o);
	}
};

class RealTime{
public:
	int minute=-404;
	RealTime()=default;
	RealTime(const RealTime&o):minute(o.minute){}
	RealTime(const int&m):minute(m){}
	RealTime(const Date&d,const Time&t){minute=(d.dayNum()-1)*24*60+t.hour*60+t.minute;}
	RealTime(const Date&d){minute=(d.dayNum()-1)*24*60;}
	RealTime(const Time&t){minute=t.hour*60+t.minute;}
	
	Date date()const{
		int day=minute/24/60+1,month=6;
		while(day>days[month-5]) month++;
		return Date(month,day-days[month-6]);
	}
	Time time()const{
		int m=minute%(60*24);
		return Time(m/60,m%60);
	}
	string show_message()const{
		if(minute<0) return "xx-xx xx:xx";
		return date().show_message()+" "+time().show_message();
	}
	
	RealTime&operator=(const RealTime&o){
		minute=o.minute;
		return *this;
	}
	RealTime operator+(const int&x)const{
		return RealTime(minute+x);
	}
	RealTime operator+(const RealTime&o)const{
		return RealTime(minute+o.minute);
	}
	RealTime operator-(const int&x)const{
		return RealTime(minute-x);
	}
	int operator-(const RealTime&o)const{
		return minute-o.minute;
	}
	bool operator<(const RealTime&o)const{return minute<o.minute;}
	bool operator==(const RealTime&o)const{return minute==o.minute;}
	bool operator<=(const RealTime&o)const{return minute<=o.minute;}
};
#endif nnnd
