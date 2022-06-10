#include <iostream>
#include "BPlusTree.hpp"
#include "String.hpp"
#include <algorithm>
using namespace Geneva;
String str("chao");
BPlusTree<50,50,250> Q("test");
void test(){
    String key=str;
    for(int i=1;i<=400;i++) {
        Q.insert(std::make_pair(str, i), i);
    }
   Q.show();
    for(int i=400;i>0;i-=2){
        Q.erase(std::make_pair(str,i));

    }
    Q.show();
    std::pair<long long*,int>pat(Q.find(key));
    if(pat.second==0)std::cout<<"null";
    else std::sort(pat.first,pat.first+pat.second);
    for(int i=0;i<pat.second;i++){
        std::cout<<pat.first[i]<<' ';
    }
    std::cout<<std::endl;

}
void pour(){
    String ts("Tay");
    String jzm("Jiang");
    String diana("JiaRan");
    for(int i=1989;i<2000;i++)Q.insert(std::make_pair(ts,i),i);
    for(int i=19260817;i<19260831;i++)Q.insert(std::make_pair(jzm,i),i);
    for(int i=1;i<=520;i++)Q.insert(std::make_pair(diana,i),i);
}
void test2(){
    for(int i=1;i<=20;i++){
        Q.insert(std::make_pair(str,i),i);
    }
    Q.show();

    for(int i=40;i>20;i--){
        Q.insert(std::make_pair(str,i),i);
    }
    Q.show();
    String key=str;
    for(int i=19;i>0;i-=2){
        Q.erase(std::make_pair(str,i));
    }
    for(int i=21;i<40;i+=2){
        Q.erase(std::make_pair(str,i));
    }
    std::pair<long long*,int>pat2(Q.find(key));
    if(pat2.second==0)std::cout<<"null";
    else std::sort(pat2.first,pat2.first+pat2.second);
    for(int i=0;i<pat2.second;i++){
        std::cout<<pat2.first[i]<<' ';
    }
}
int main() {
    int n;std::cin>>n;
    if(n==0)pour();
    while(n--){
        std::string command;
        std::cin>>command;
        if(command=="insert"){
            String key;
            int value;
            std::cin>>key>>value;
            Q.insert(std::make_pair(key,value),1);
        }
        if(command=="delete"){
            String key;
            int value;
            std::cin>>key>>value;
            Q.erase(std::make_pair(key,value));
        }
        if(command=="find"){
            String key;
            std::cin>>key;
            std::pair<long long*,int>pat(Q.find(key));
            if(pat.second==0)std::cout<<"null";
            else std::sort(pat.first,pat.first+pat.second);
            for(int i=0;i<pat.second;i++){
                std::cout<<pat.first[i]<<' ';
            }

            std::cout<<std::endl;
        }

    }

}
