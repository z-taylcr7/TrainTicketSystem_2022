#include "User.h"
class UserManager{
private:
	class UserStorage{
	private:
struct ints {int value=0; ints()=default; explicit ints(int x):value(x){}};
		BPlusTree<> user_index;
		MemoryPool<User,ints>*user_data;
		bool is_empty=1;
	public:
		void printbacis(){user_index.printbacis();}
		void rollback(const int&ddl){
			user_index.rollback(ddl);
			user_data->rollback(ddl);
		}
		UserStorage():user_index("user_index.dat"){
			user_data=new MemoryPool<User,ints>("user_data.dat",ints(0),1000000/sizeof(User));
			is_empty=(user_data->readExtraBlock().value==0);
		}
		~UserStorage(){
			delete user_data;
		}
		
		int get_id(const string&username){// �ҵ�username���˻��Ĵ����ַ 
			vector<pair<int,ll> >temp;
			user_index.find(username,temp);
			if(temp.empty()) return -404;
			return temp[0].first;
		}
		User get_user(int id){return user_data->get(id);}
		
		void update(const int&id,const User&user){user_data->update(id,user);}
		void add_user(const string&username,const User&user){
			int id=user_data->add(user);
//			std::cout<<"<id = "<<id<<" >" <<std::endl;
			user_index.insert({username,id},id); 
//			puts("users_insert finished");
		}
		
		bool empty(){return is_empty;}
		void not_empty(){user_data->writeExtraBlock(ints(1));is_empty=0;}
		void clean(){
			user_data->clearAll();
			user_index.clear();
		}
	}users;
	HashMap<string,std::pair<int,int> > logged_users;
public:
	UserManager(){}
	~UserManager(){logged_users.clear();}
	void printbacis(){users.printbacis();}
	bool add_user(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege){
//		std::cout<<username<<"  GG\n"; 
		
		if(users.empty()){
//			puts("!!");std::cout<<username<<"  GG\n"; 
			if(!logged_users.empty()){
//				std::cout<<"wtf\n";
				return 0;//Unknown situation 
			}
//			puts("!!!");
			User user(username,password,name,mailAddr,10);
//			puts("!!");
			users.add_user(username,user);
//			puts("!");
			users.not_empty();
			return 1;
		}
		int priority=check_priority(ouser);
		if(priority==-404 || priority<=privilege){
//			std::cout<<"here1\n";
			return 0;// �û������ڻ�Ȩ�޲��� 
		}
		if(users.get_id(username)>=0){
//			std::cout<<"here2\n";
			return 0; //�û��Ѵ��� 
		}
		users.add_user(username,User(username,password,name,mailAddr,privilege));
		return 1; 
	}
	
	bool login(const string&username,const string&password){
		if(check_priority(username)!=-404) return 0;// �ѵ�¼ 
		int id=users.get_id(username);
		if(id<0){
//			std::cout<<"GGhere"<<id<<std::endl;
			return 0;//�û������� 
		} 
		User user(users.get_user(id));
		if(!user.check_pass(password)){
 //           std::cout<<"gghere hhh"<<std::endl;
            return 0;//�������
        }
		logged_users.insert(username,{user.priority(),user.query_ticketNum()});
		return 1;
	}
	bool logout(const string&username){
//		std::cout<<"! "<<username<<" !\n";
		auto tmp=logged_users.find(username);
		if(tmp==NULL) return 0;//�û������ڻ�δ��¼ 
		logged_users.erase(username);
		return 1; 
	}
	
	string query_profile(const string&ouser,const string&username){
		string fail("-1");
		int priority=check_priority(ouser);
		if(priority==-404){
 //           std::cout<<"weidenglu"<<std::endl;
            return fail;//δ��¼
        }
		
		int id=users.get_id(username);
		if(id<0){
//            std::cout<<"meizhaodao"<<std::endl;
            return fail;//�û�������
        }
		User user(users.get_user(id));
		if(user.priority()>=priority&&ouser!=username) return fail;//Ȩ�޲���
		return user.ID_message(); 
	}
	string modify_profile(const string&ouser,const string&username,const string&password,const string&name,const string&mailAddr,const int&privilege){
		string fail("-1");
		int priority=check_priority(ouser);
		if(priority==-404){
//			std::cout<<"meidenglu"<<std::endl;
			return fail;//δ��¼
		}
		int id=users.get_id(username);
		if(id<0){
//			std::cout<<"zhaobudao"<<std::endl;
			return fail;//�û�������
		}
		User user(users.get_user(id));
		if((user.priority()>=priority&&ouser!=username)||privilege>=priority){
//			std::cout<<user.priority()<<"wtf???"<<priority<<std::endl;
			return fail;//Ȩ�޲���
		}
		if(password!="") user.pass()=password;
		if(name!="") user.nam()=name;
		if(mailAddr!="") user.mail()=mailAddr;
		if(privilege!=-404) user.priority()=privilege;
		users.update(id,user);
		return user.ID_message();
	}

    int check_priority(const string&user){
        auto tmp=logged_users.find(user);
        if(tmp==NULL) return -404;
        int id=users.get_id(user);
        User xxx(users.get_user(id));
        return xxx.priority();
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
	void rollback(const int&ddl){
		users.rollback(ddl);
		logged_users.clear();
	}
};
