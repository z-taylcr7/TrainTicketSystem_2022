#include "User.h"
class UserManager{
private:
	class UserStorage{
	private:
struct ints {int value=0; ints()=default; explicit ints(int x):value(x){}};
		StoragePool<User,ints,100>user_data;
		BPlusTree<100,200>user_index;
		bool is_empty=1;
	public:
		UserStorage():user_data("user_data.dat"),user_index("user_index.dat"){
			is_empty=(user_data.readExtraBlock().value==0);
		}
		~UserStorage()=default;
		
		int get_id(const string&username){// 找到username的账户的储存地址 
			vector<pair<int,ll> >temp;
			user_index.find(username,temp);
			if(temp.empty()) return -404;
			return temp[0].first;
		}
		User get_user(int id){return user_data.get(id);}
		
		void update(const int&id,const User&user){user_data.update(id,user);}
		void add_user(const string&username,const User&user){
			int id=user_data.add(user);
			user_index.insert({username,id},id); 
		}
		
		bool empty(){return is_empty;}
		void not_empty(){user_data.writeExtraBlock(ints(1));is_empty=0;}
		void clean(){
			user_data.clearAll();
			user_index.clean();
		}
	}users;
	HashMap<string,std::pair<int,int> > logged_users;
public:
	UserManager(){}
	~UserManager(){logged_users.clear();}
	
	bool add_user(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege){
//		std::cout<<username<<'\n'; 
		if(users.empty()){
//			puts("!!");
			if(!logged_users.empty()) return 0;//Unknown situation 
//			puts("!!!");
			User user(username,password,name,mailAddr,10);
			users.add_user(username,user);
			users.not_empty();
			return 1;
		}
		int priority=check_priority(ouser);
		if(priority==-404 || priority<=privilege) return 0;// 用户不存在或权限不够 
		if(users.get_id(username)>=0) return 0; //用户已存在 
		users.add_user(username,User(username,password,name,mailAddr,privilege));
		return 1; 
	}
	
	bool login(const string&username,const string&password){
		if(check_priority(username)!=-404) return 0;// 已登录 
		int id=users.get_id(username);
		if(id<0) return 0;//用户不存在 
		User user(users.get_user(id));
		if(!user.check_pass(password)) return 0;//密码错误 
		logged_users.insert(username,{user.priority(),user.query_ticketNum()});
		return 1;
	}
	bool logout(const string&username){
//		std::cout<<"! "<<username<<" !\n";
		auto tmp=logged_users.find(username);
		if(tmp==NULL) return 0;//用户不存在或未登录 
		logged_users.erase(username);
		return 1; 
	}
	
	string query_profile(const string&ouser,const string&username){
		string fail("-1");
		int priority=check_priority(ouser);
		if(priority==-404) return fail;//未登录 
		
		int id=users.get_id(username);
		if(id<0) return fail;//用户不存在
		User user(users.get_user(id));
		if(user.priority()>=priority&&ouser!=username) return fail;//权限不够
		return user.ID_message(); 
	}
	string modify_profile(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege){
		string fail("-1");
		int priority=check_priority(ouser);
		if(priority==-404) return fail;//未登录
		
		int id=users.get_id(username);
		if(id<0) return fail;//用户不存在
		User user(users.get_user(id));
		if((user.priority()>=priority&&ouser!=username)||privilege>=priority) return fail;//权限不够
		if(password!="") user.pass()=password;
		if(name!="") user.nam()=name;
		if(mailAddr!="") user.mail()=mailAddr;
		if(privilege!=-404) user.priority()=privilege;
		users.update(id,user);
		return user.ID_message();
	}
	
	int check_priority(const string&user)const{
		auto tmp=logged_users.find(user);
		if(tmp==NULL) return -404;
		return tmp->first;
	}
	void clean(){
		users.clean();
		logged_users.clear();
	}
	
	int query_priority(const string&user){
		int id=users.get_id(user);
		if(id<0) return -404;
		return users.get_user(id).priority();
	}
	int query_ticketNum(const string&user)const{
		auto tmp=logged_users.find(user);
		return tmp->second;
	}
	bool add_ticketNum(const string&username){
		int id=users.get_id(username);
		if(id<0) return 0;
		User user(users.get_user(id));
		user.add_ticket();
		users.update(id,user);
		
		auto tmp=logged_users.find(username);
		if(tmp!=NULL) tmp->second++;
		return 1;
	}
};
