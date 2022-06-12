//
// Created by Cristiano on 2022/5/13.
//

#ifndef BPLUSTREE_MEMORYPOOL_HPP
#define BPLUSTREE_MEMORYPOOL_HPP
#include <fstream>
#include "hashmap.hpp"

namespace Geneva{
template<class T,class V>
    class MemoryPool{
        class DoublyLinklist{
        public:
            class node{
            public:
                int key=-1;
                T* value;
                node*pre,*nxt;
                node():pre(nullptr),nxt(nullptr){};
                node(const int&k,const T &v,node*pr= nullptr,node* nx= nullptr):key(k),value(new T(v)),pre(pr),nxt(nx){}
                bool refresh= false;
            };
            node *head= nullptr;
            node* tail= nullptr;
            int sum=0;
            int cap=0;
            explicit DoublyLinklist(int _cap=2) : cap(_cap) {
                head = new node(), tail = new node();
                head->nxt = tail, tail->pre = head;
            }

            ~DoublyLinklist() {
                node *temp = head;
                while (head != nullptr) {
                    head = head->nxt;
                    delete temp;
                    temp = head;
                }
            }

            void clear() {
                sum = 0;
                node *temp = head;
                while (head != nullptr) {
                    head = head->nxt;
                    delete temp;
                    temp = head;
                }
                head = new node(), tail = new node();
                head->nxt = tail, tail->pre = head;
            }

            void push_front(node *n) {
                head->nxt->pre = n;
                n->nxt = head->nxt;
                head->nxt = n;
                n->pre = head;
                sum++;
            }

            void to_front(node *n) {
                n->pre->nxt = n->nxt;
                n->nxt->pre = n->pre;
                sum--;
                push_front(n);
            }

            node *pop_back() {
                node *target = tail->pre;
                target->pre->nxt = tail;
                tail->pre = tail->pre->pre;
                sum--;
                return target;
            }

            void erase(node *n) {
                n->pre->nxt = n->nxt;
                n->nxt->pre = n->pre;
                sum--;
                delete n;
            }

            bool IsFull() const {
                return sum == cap;
            }


        };
    private:
        using node_t= typename DoublyLinklist::node;
        std::string file_name;
        std::fstream file;
        V preface;
        DoublyLinklist LRUcache;
        HashMap<int, node_t* > hashmap;
    private:
        int unit = sizeof(T);
        int pre = sizeof (V);
        int writePoint=-1;
        int cachesize;
    public:
        explicit MemoryPool(std::string fn,const V&v,int size):preface(v),cachesize(size),hashmap(){
            file_name=fn;
            initialise(fn);
        }
        ~MemoryPool(){
            node_t *now = LRUcache.head->nxt;
            while (now != LRUcache.tail) {
                if (now->refresh)updateInFile(now->key, *now->value);
                now = now->nxt;
            }
        }
        int getWritePoint() {
            if(writePoint>=0)return writePoint;
            else{
                file.open(file_name);
                    file.seekp(std::ios::end);
                   int offset = file.tellp();
                   file.close();
                   return offset;
            }
        }
        bool initialise(std::string FN) {
            if (FN != "") file_name = FN;
            bool x;
            file.open(file_name,std::ios::in);
            if(!file){
                x=true;
                file.close();
                file.open(file_name,std::ios::out);
                file.seekp(0,std::ios::beg);
                writePoint=-1;
                file.write(reinterpret_cast<const char*>(&preface),sizeof(V));
                file.seekp(pre,std::ios::beg);
                file.write(reinterpret_cast<const char*>(&writePoint),sizeof(int));
            }else{
                x=false;
                file.seekg(pre,std::ios::beg);
                file.read(reinterpret_cast<char*>(&writePoint),sizeof(int));//writePoint赋值回到上一次的最后位置
            }
            file.close();
            return x;
        };
    public:

        bool existInCache(const int& key){
            return hashmap.count(key);
        }
        void eraseInCache(const int&key){
            LRUcache.erase(hashmap[key]);
            hashmap.erase(key);
        }
        void putInCache(int key, const T &data) {
            if (existInCache(key)) {
                LRUcache.to_front(hashmap[key]);
                *hashmap[key]->value = data;
                return;
            }
            if (LRUcache.IsFull())discardLRU();
            auto newnode= new node_t (key,data);
            LRUcache.push_front(newnode);
            hashmap[key] =newnode;
        }
        void discardLRU() {
            node_t *target = LRUcache.pop_back();
            hashmap.erase(target->key);
            if (target->refresh)updateInFile(target->key, *target->value);
            delete target;
        }
        int writeInFile(const T& t){
            int offset;
            file.open(file_name);
            if (writePoint <= 0) {
                file.seekp(std::ios::end);
                offset = file.tellp();
            }
            else {
                offset = writePoint;
                file.seekp(writePoint,std::ios::beg);
                file.read(reinterpret_cast<char *>(&writePoint), sizeof(int));
                file.seekp(pre, std::ios::beg);
                file.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
                file.seekp(offset,std::ios::beg);
            }
            file.write(reinterpret_cast<const char *>(&t), sizeof(T));
            file.close();
            return offset;
        } 
        void updateInFile(int index, T &t) {
            file.open(file_name);
            file.seekp(index,std::ios::beg);
            file.write(reinterpret_cast<char *>(&t), unit);
            file.close();
        };
         T readInFile(int index){
            file.open(file_name);
            T t;
            file.seekg(index);
            file.read(reinterpret_cast<char *>(&t), unit);
            file.close();
            return t;
        };
         void eraseInFile(int index){
            // todo
            file.open(file_name);
            file.seekp(index,std::ios::beg);
            file.write(reinterpret_cast<const char*>(&writePoint),sizeof(int));
            writePoint=index;
            file.seekp(pre,std::ios::beg);
            file.write(reinterpret_cast<const char*>(&writePoint),sizeof(int));
        };
    public:
         V readPre(){
             file.open(file_name);
             V v;
             file.seekp(0,std::ios::beg);
             file.read(reinterpret_cast<char*>(&v),sizeof(V));//writePoint赋值回到上一次的最后位置
             file.close();
             return v;
         }
        void updatePre(const V &v) {
            file.open(file_name);
            file.seekp(0,std::ios::beg);
            file.write(reinterpret_cast<const char *>(&v),pre);
            file.close();
        }
        T read(int offset) {
            T tmp = existInCache(offset) ? *(hashmap[offset]->value): readInFile(offset);
            putInCache(offset, tmp);
            return tmp;
        }

        int write(const T &t) {
            int offset = writeInFile(t);
            putInCache(offset, t);
            return offset;
        }

        void update(const T &o, int offset) {
            putInCache(offset, o);
            hashmap[offset]->refresh = true;
        }

        void erase(int offset) {
            if (existInCache(offset))eraseInCache(offset);
            eraseInFile(offset);
        }

        void clear(){
            //todo
            LRUcache.clear();
            hashmap.clear();
            writePoint=-1;
            file.open(file_name);
            file.seekp(0,std::ios::beg);
            file.write(reinterpret_cast<const char* >(&preface),pre);
            file.seekp(pre,std::ios::beg);
            file.write(reinterpret_cast<const char*>(writePoint),sizeof(int)) ;
            file.close();


        }
    };
}
#endif //BPLUSTREE_MEMORYPOOL_HPP
