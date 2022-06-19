//
// Created by walotta on 2021/5/14.
//

#include "System.h"
#include <sstream>
typedef const string & str;
typedef const vector<int> & vecI;
typedef vector<string> vecS;
typedef long long lint;
typedef std::pair<int,lint> ex_index;
int tim=0;

class separator{
    std::string line;
    vector<std::string> tokens;
    char ban;

public:
    separator()=default;
    explicit separator(char ch):ban(ch){}
    ~separator(){
        tokens.clear();
    }
    void process(){
        tokens.clear();
        int cnt=0;
        std::string token;
        for(int i=0;i<line.size();i++){
            if(line[i]==ban){
                if(token.size()){
                    tokens.push_back(token);
                    token.clear();++cnt;
                }
            }else{
                token+=line[i];
            }
        }
        if(token.size()){
            tokens.push_back(token);
            token.clear();++cnt;
        }
    };
    void process(const std::string& l){
        line=l;
        process();
    }
    int size(){
        return tokens.size();
    }
    bool empty(){
        return tokens.empty();
    }
    bool readLine(std::istream &is)
    {
        if(is.eof()) return false;
        getline(is,line);
        process();
        return true;
    }
    vector<std::string> content(){
        return tokens;
    }
    std::string &operator[](int p){
        return tokens[p];
    }

};
class cmd
{
private:
    System sys;
    separator words;

    static void to_vector_int(str input,vector<int> &out)
    {
        separator ints('|');
        ints.process(input);
        for(int i=0;i<ints.size();++i) out.push_back(to_int(ints[i]));
    }
    static void to_vector_str(str input,vector<string> &out)
    {
        separator ints('|');
        ints.process(input);
        out=ints.content();
    }

    template<class T>
    void print_os(const vector<T> &list,ostream &ost)
    {
        int size=list.size();
        for(int i=0; i<size; i++)
        {
            ost<<list[i];
            if(i!=size-1) ost<<'\n';
        }
    }
public:
    cmd():words(' '){}
    int To_int(string s){
    	int res=0,len=s.length(),i=0;
    	while(s[i]>'9'||s[i]<'0') i++;
    	while(i<len&&s[i]>='0'&&s[i]<='9') res=res*10+s[i]-'0',i++;
    	return res;
    }
    void run(std::istream &is, std::ostream &os)
    {
//    	puts("Programme begin");
//    	int tt=4;
        words.readLine(is);
        do
        {
//            sys.printbacis();
//        	std::cout<<"!!!"<<'\n';
//        	tt--;if(!tt) break;
            if(words.empty()) continue;
//            std::cerr<<"[Debug]"<<words[1]<<std::endl;
            auto &command=words[1];
            os<<words[0]<<" "; 
            tim=To_int(words[0]);
            if(command=="add_user")
            {
                std::string c(""),u(""),p(""),n(""),m("");
                int g=-404;
                for(int i=2; i<words.size(); i+=2)
                {
                    if(words[i]=="-c") c=words[i+1];
                    else if(words[i]=="-u") u=words[i+1];
                    else if(words[i]=="-p") p=words[i+1];
                    else if(words[i]=="-n") n=words[i+1];
                    else if(words[i]=="-m") m=words[i+1];
                    else if(words[i]=="-g") g=to_int(words[i+1]);
                }
//                std::cout<<u<<std::endl;
                os<<sys.add_user(c,u,p,n,m,g)-1;
            }
            else if(command=="login")
            {
                std::string u(""),p("");
                for(int i=2; i<words.size(); i+=2)
                {
                    if(words[i]=="-u") u=words[i+1];
                    else p=words[i+1];
                }
                os<<sys.login(u,p)-1;
            }
            else if(command=="logout")
            {
                os<<sys.logout(words[3])-1;
            }
            else if(command=="query_profile")
            {
                std::string c(""),u("");
                for(int i=2; i<words.size(); i+=2)
                {
                    if(words[i]=="-c") c=words[i+1];
                    else u=words[i+1];
                }
                os<<sys.query_profile(c,u);
            }
            else if(command=="modify_profile")
            {
                std::string c(""),u(""),p(""),n(""),m("");
                int g=-404;
                for(int i=2; i<words.size(); i+=2)
                {
                    if(words[i]=="-c") c=words[i+1];
                    else if(words[i]=="-u") u=words[i+1];
                    else if(words[i]=="-p") p=words[i+1];
                    else if(words[i]=="-n") n=words[i+1];
                    else if(words[i]=="-m") m=words[i+1];
                    else if(words[i]=="-g") g=to_int(words[i+1]);
                }
                os<<sys.modify_profile(c,u,p,n,m,g);
            }
            else if(command=="add_train")
            {
                string i;
                int n,m;
                Date d_beg,d_end;
                Time x;
                char y;
                vector<string> s;
                vector<int> p,t,o;
                for(int j=2; j<words.size(); j+=2){ 
                    if(words[j]=="-n") n=to_int(words[j+1]);
                } 
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-i") i=words[j+1];
                    else if(words[j]=="-m") m=to_int(words[j+1]);
                    else if(words[j]=="-s") to_vector_str(words[j+1],s);
                    else if(words[j]=="-p") to_vector_int(words[j+1],p);
                    else if(words[j]=="-x") x=Time(words[j+1]);
                    else if(words[j]=="-t") to_vector_int(words[j+1],t);
                    else if(words[j]=="-o") {if(n>2) to_vector_int(words[j+1],o);}
                    else if(words[j]=="-d")
                    {
                        string &day=words[j+1];
                        d_beg=Date(day.substr(0,5));
                        d_end=Date(day.substr(6,5));
                    }
                    else if(words[j]=="-y") y=words[j+1][0];
                }
                os<<sys.add_train(i,n,m,s,p,x,t,o,d_beg,d_end,y)-1;
//                if(y<'A'||y>'Z'){
//	                for(int j=2; j<words.size(); j+=2)
//	                {
//	                    if(words[j]=="-y"){
////	                    	std::cout<<words[j]<<" "<<words[j+1]<<"! "<<words[j+1][0]<<"!! "<<y<<"       ";
//	                    	os<<sys.add_train(i,n,m,s,p,x,t,o,d_beg,d_end,words[j+1][0])-1;
//	                    	break;
////							y=words[j+1][0];
//						}
//	                }
//                	
//                }
//                else os<<sys.add_train(i,n,m,s,p,x,t,o,d_beg,d_end,y)-1;
            }
            else if(command=="release_train")
            {
                os<<sys.release_train(words[3])-1;
            }
            else if(command=="query_train")
            {
                string i;
                Date d;
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-i") i=words[j+1];
                    else d=Date(words[j+1]);
                }
                static vecS ans;
                ans.clear();
                sys.query_train(i,d,ans);
                print_os(ans,os);
            }
            else if(command=="delete_train")
            {
                os<<sys.delete_train(words[3])-1;
            }
            else if(command=="query_ticket")
            {
//            	continue;
                string s,t;
                Date d;
                bool p=true;
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-s") s=words[j+1];
                    else if(words[j]=="-t") t=words[j+1];
                    else if(words[j]=="-d") d=Date(words[j+1]);
                    else if(words[j]=="-p")
                    {
                        if(words[j+1]=="time") p=true;
                        else p=false;
                    }
                }
                static vecS ans;
                ans.clear();
                sys.query_ticket(s,t,d,p,ans);
                print_os(ans,os);
            }
            else if(command=="query_transfer")
            {
                string s,t;
                Date d;
                bool p=true;
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-s") s=words[j+1];
                    else if(words[j]=="-t") t=words[j+1];
                    else if(words[j]=="-d") d=Date(words[j+1]);
                    else if(words[j]=="-p")
                    {
                        if(words[j+1]=="time") p=true;
                        else p=false;
                    }
                }
                static vecS ans;
                ans.clear();
                sys.query_transfer(s,t,d,p,ans);
                print_os(ans,os);
            }
            else if(command=="buy_ticket")
            {
//            	continue;
                string u,i,f,t;
                Date d;
                int n;
                bool q=false;
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-u") u=words[j+1];
                    else if(words[j]=="-i") i=words[j+1];
                    else if(words[j]=="-d") d=Date(words[j+1]);
                    else if(words[j]=="-n") n=to_int(words[j+1]);
                    else if(words[j]=="-f") f=words[j+1];
                    else if(words[j]=="-t") t=words[j+1];
                    else if(words[j]=="-q")
                    {
                        if(words[j+1]=="false") q=false;
                        else q=true;
                    }
                }
                os<<sys.buy_ticket(u,i,d,f,t,n,q);
            }
            else if(command=="query_order")
            {
                string u("");
                if(words[2]=="-u") u=words[3];
                static vecS ans;
                ans.clear();
                sys.query_order(u,ans);
                print_os(ans,os);
            }
            else if(command=="refund_ticket")
            {
                string u;
                int n=1;
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-u") u=words[j+1];
                    if(words[j]=="-n") n=to_int(words[j+1]);
                }
                os<<sys.refund_ticket(u,n)-1;
            }
            else if(command=="clean")
            {
                sys.clean();
                os<<0;
            }
            else if(command=="exit")
            {
                os<<"bye"<<std::endl;
                os.flush();
                break;
            }
            else if(command=="rollback")
            {
            	int timn=0;
                for(int j=2; j<words.size(); j+=2)
                {
                    if(words[j]=="-t") timn=to_int(words[j+1]);
                }
                if(timn>tim) os<<-1;
                else os<<sys.rollback(timn);
            }
            os<<"\n";
        } while(words.readLine(is));
    }
};
