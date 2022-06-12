//
// Created by Cristiano on 2022/5/22.
//
#include <cstring>
#ifndef BPLUSTREE_HASHMAP_HPP
#define BPLUSTREE_HASHMAP_HPP
namespace Geneva{
template<typename key_type,typename value_type,class Hash=std::hash<key_type>>
class HashMap2
{
private:
    typedef unsigned long long unsigned_lint;

    static const unsigned_lint prime_number=30;
    const unsigned_lint prime_list[prime_number]={
            53,101,211,401,809,1601,3203,6421,12809,25601,51203,102407,204803,409609,//14
            819229,1638431,3276803,6553621,13107229,26214401ll,52428841ll,104857601ll,209715263ll,//9
            419430419ll,838860817ll,1677721631ll,3355443229ll,6710886407ll,13421772823ll,26843545607ll//7
    };

    struct Node
    {
        key_type key;
        value_type value;
        Node *next;

        Node(const key_type &k,const value_type &v,Node *n):key(k),value(v),next(n){}
        ~Node()=default;
    };
    struct LinkList
    {
        Node *head;
        unsigned_lint length;

        LinkList():head(nullptr),length(0){}
        ~LinkList()
        {
            while(head!=nullptr)
            {
                auto temp=head;
                head=head->next;
                delete temp;
            }
        }

        void insert(const key_type &key,const value_type &value)
        {
            Node *new_head=new Node(key,value,head);
            head=new_head;
            ++length;
        }
        void erase(const key_type &key)
        {
            if(head==nullptr) return;
            if(head->key==key)
            {
                auto temp=head;
                head=head->next;
                delete temp;
                --length;
                return;
            }
            auto now=head;
            while(now->next!=nullptr)
            {
                if(now->next->key==key)
                {
                    auto temp=now->next;
                    now->next=temp->next;
                    delete temp;
                    --length;
                    return;
                }
                now=now->next;
            }
        }
        Node *find(const key_type &key)
        {
            auto now=head;
            if(now==nullptr) return nullptr;
            while(now->key!=key && now->next!=nullptr) now=now->next;
            if(now->key==key) return now;
            return nullptr;
        }

        unsigned_lint size() const {return length;}
        bool empty() const {return length==0;}
        void add_node(Node *node)
        {
            node->next=head;
            head=node;
            ++length;
        }
    };

    unsigned_lint prime_pos_now=0;
    unsigned_lint length=0;
    LinkList *lists;
    Hash hash;

    inline unsigned_lint get_index(const key_type &key) const
    {
        return hash(key)%prime_list[prime_pos_now];
    }

    void resize()
    {
        auto *new_list=new LinkList[prime_list[++prime_pos_now]];
        for(int i=0;i<prime_list[prime_pos_now-1];++i)
        {
            Node *now=lists[i].head;
            while(now!=nullptr)
            {
                auto pos=get_index(now->key);
                auto temp=now;
                now=now->next;
                new_list[pos].add_node(temp);
            }
            lists[i].head=nullptr;
        }
        delete [] lists;
        lists=new_list;
    }
public:
    HashMap2():prime_pos_now(0),length(0)
    {
        lists=new LinkList[prime_list[prime_pos_now]];
    }
    ~HashMap2() {delete [] lists;}

    void insert(const key_type &key,const value_type &value)
    {
        if(length==prime_list[prime_pos_now]) resize();
        auto pos=get_index(key);
        ++length;
        lists[pos].insert(key,value);
    }

    void erase(const key_type &key)
    {
        auto pos=get_index(key);
        lists[pos].erase(key);
        --length;
    }

    unsigned_lint count(const key_type &key)
    {
        auto pos=get_index(key);
        if(lists[pos].find(key)==nullptr) return 0;
        return 1;
    }

    value_type &operator[](const key_type &key)
    {
        if (lists[get_index(key)].find(key)==NULL) insert(key,value_type());
        return lists[get_index(key)].find(key)->value;
    }

    value_type *find(const key_type &key) const
    {
        auto pos=get_index(key);
        if(lists[pos].empty()) return nullptr;
        Node *out=lists[pos].find(key);
        if(out==nullptr) return nullptr;
        return &(out->value);
    }

    bool empty() const {return length==0;}

    void clear()
    {
        delete [] lists;
        prime_pos_now=0;
        length=0;
        lists=new LinkList[prime_list[prime_pos_now]];
    }
};

}
#endif //BPLUSTREE_HashMap2_HPP
