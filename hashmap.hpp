//
// Created by Cristiano on 2022/5/22.
//
#include <cstring>
#ifndef BPLUSTREE_HASHMAP_HPP
#define BPLUSTREE_HASHMAP_HPP
namespace Geneva{
    template<class Key,class Data>
    class HashMap{
        static const int N=19260817;
    public:
        class node{
        public:
            Key key;Data data;
            node*nxt;
            node():nxt(nullptr){};
            node(const Key&k,const Data &d,node* nx):key(k),data(d),nxt(nx){}
        };
        std::hash<Key> hash;
        node* heads[N];
        inline unsigned long long getHash(const Key& k){
            return hash(k)%N;
        }
        explicit HashMap(){
            memset(heads,0,sizeof(heads));
        }
        ~HashMap(){
            for(int i=0;i<N;i++){
                for(node*p=heads[i],*q;p!= nullptr;p=q){
                    q=p->nxt;
                    delete p;
                }
            }
        }
        void clear(){
            for(int i=0;i<N;i++){
                if(heads[i]->nxt)
                {
                    for(node*p=heads[i]->nxt,*q;p!= nullptr;p=q){
                        q=p->nxt;
                        delete p;
                    }
                }
            }
        }
        void insert(const Key& k,const Data&d){
            size_t pos= getHash(k);
            heads[pos]=new node(k,d,heads[pos]);
        }
        Data &operator[](const Key&k){
            size_t pos= getHash(k);
            for (node *p = heads[pos]; p; p = p->nxt){
                if(p->key==k)return p->data;
            }
            insert(k,Data());
            return heads[pos]->data;
            }
        void erase(const Key&k){
            size_t pos= getHash(k);
            if(heads[pos]&&heads[pos]->key==k){
                node* del=heads[pos];
                heads[pos]=heads[pos]->nxt;delete del;return;
            }
            for (node *p = heads[pos]; p; p = p->nxt){
                if (p->nxt && p->nxt->key == k){
                    node *del = p->nxt;
                    p->nxt = del->nxt;
                    delete del; break;
                }
            }
        }
        int count(const Key &key) {
            unsigned long long x=getHash(key);
            for (node *p = heads[x]; p; p = p->nxt){
                if (p->key == key) return 1;
            }
            return 0;
        }

    };
}
#endif //BPLUSTREE_HASHMAP_HPP
