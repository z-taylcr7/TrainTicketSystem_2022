#include "base.h"
class User{
private:
	MyString username,password,name,mailAddr;
	int privilege;// [0,10]
	int ticketNum=0;
public:
	User()=default;
	User(const User&o):username(o.username),password(o.password),name(o.name),mailAddr(o.mailAddr),privilege(o.privilege),ticketNum(o.ticketNum){}
	User(const string&_username,const string&_password,const string&_name,const string&_mailAddr,const int&_privilege):username(_username),password(_password),name(_name),mailAddr(_mailAddr),privilege(_privilege){}
	
	string ID_message()const{return (string)username+" "+(string)name+" "+(string)mailAddr+" "+std::to_string(privilege);}
	bool check_pass(string _password){
 //       std::cout<<password<<std::endl;
        return password==_password;
    }
	
	MyString&user(){return username;}
	MyString&pass(){return password;}
	MyString&nam(){return name;}
	MyString&mail(){return mailAddr;} 
	int&priority(){return privilege;}
	int query_ticketNum ()const{return ticketNum;}
	void add_ticket(){ticketNum++;}
};
