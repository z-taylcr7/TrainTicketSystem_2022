#include "ErrorMessage.h"
template<int cacheSize,class T>
class cachePool{
private:
    int id[cacheSize];
    T* storage[cacheSize];
    int size;
    int head;
public:
    cachePool():size(0),head(-1){}
    ~cachePool(){
        for(int i=0;i<size;i++) delete storage[i];
    }
    int find(int _id){
        for(int i=0;i<size;i++)
            if(id[i]==_id) return i;
        return -1;
    }
    T operator[](int pos){return *(storage[pos]);}
    void update(int _id,const T& other){
        int pos=find(_id);
        if(pos==-1) insert(_id,other);
        else{
            delete storage[pos];
            storage[pos]=new T(other);
        }
    }
    void insert(int _id,const T& other){
        if(size==cacheSize){
            head++;
            head%=size;
            delete storage[head];
            storage[head]=new T(other);
            id[head]=_id;
        }else{
            head++;
            size++;
            storage[head]=new T(other);
            id[head]=_id;
        }
    }
    void remove(int _id){
        int pos;
        pos=find(_id);
        if(pos==-1)return;
        id[pos]=-2;
    }
    void clear(){
        for(int i=0;i<size;i++) delete storage[i];
        size=0;
        head=-1;
    }
};
