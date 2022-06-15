//
// Created by Cristiano on 2022/6/3.
//

#ifndef BPLUSTREE_LRUMEMORYPOOL_HPP
#define BPLUSTREE_LRUMEMORYPOOL_HPP
#include <iostream>
#include <fstream>
#include "STL.h"
using std::pair;
using std::string;

namespace Geneva {
    template<class T, class preface = int>
    class MemoryPool {
    private:
        class DoublyLinkedList {
        public:
            class Node {
            public:
                int key = -1;
                T *value = nullptr;
                Node *pre = nullptr;
                Node *next = nullptr;
                bool useless = false;

                Node() = default;

                Node(int k, const T &v, Node *pre_ = nullptr, Node *nxt_ = nullptr) : key(k), value(new T(v)), pre(pre_),
                                                                                      next(nxt_) {}

                ~Node() {
                    delete value;
                }
            };

            Node *head = nullptr;
            Node *tail = nullptr;
            int listSize = 0;
            int capacity = 0;

            explicit DoublyLinkedList(int _capacity) : capacity(_capacity) {
                head = new Node(), tail = new Node();
                head->next = tail, tail->pre = head;
            }

            ~DoublyLinkedList() {
                Node *tmp = head;
                while (head != nullptr) {
                    head = head->next;
                    delete tmp;
                    tmp = head;
                }
            }

            void clear() {
                listSize = 0;
                Node *tmp = head;
                while (head != nullptr) {
                    head = head->next;
                    delete tmp;
                    tmp = head;
                }
                head = new Node(), tail = new Node();
                head->next = tail, tail->pre = head;
            }

            void push_front(Node *n) {
                head->next->pre = n;
                n->next = head->next;
                head->next = n;
                n->pre = head;
                listSize++;
            }

            void to_front(Node *n) {
                n->pre->next = n->next;
                n->next->pre = n->pre;
                listSize--;
                push_front(n);
            }

            Node *pop_back() {
                Node *target = tail->pre;
                target->pre->next = tail;
                tail->pre = tail->pre->pre;
                listSize--;
                return target;
            }

            void erase(Node *n) {
                n->pre->next = n->next;
                n->next->pre = n->pre;
                listSize--;
                delete n;
            }

            bool full() const {
                return listSize == capacity;
            }
        };

        using node_t = typename DoublyLinkedList::Node;
        const string filename;
        FILE *file;
        int writePoint=-1;


        HashMap<int, node_t*> hashmap;
        DoublyLinkedList cache;

        int count_Cache(int key) {
            return hashmap.count(key);
        }

        void remakeLRU() {
            node_t *target = cache.pop_back();
            hashmap.erase(target->key);
            if (target->useless)update_File(target->key, *target->value);
            delete target;
        }

        void erase_Cache(int key) {
            cache.erase(hashmap[key]);
            hashmap.erase(key);
        }

        void write_Cache(int key, const T &o) {
            if (count_Cache(key)) {
                cache.to_front(hashmap[key]);
                *hashmap[key]->value = o;
                return;
            }
            auto newNode = new node_t(key, o);
            if (cache.full())remakeLRU();
            cache.push_front(newNode);
            hashmap[key] = newNode;
        }

        int write_File(const T &o) {
            int offset;
            file = fopen(filename.c_str(), "rb+");
            if (writePoint <= 0) {
                fseek(file, 0, SEEK_END);
                offset = ftell(file);
            } else {
                offset = writePoint;
                fseek(file, writePoint, SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fseek(file, sizeof(preface), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
                fseek(file, offset, SEEK_SET);
            }
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);
            return offset;
        }

        T read_File(int offset) {
            file = fopen(filename.c_str(), "rb");
            T tmp;
            fseek(file, offset, SEEK_SET);
            fread(reinterpret_cast<char *>(&tmp), sizeof(T), 1, file);
            fclose(file);
            return tmp;
        }

        void update_File(int offset, const T &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);
        }

        void erase_File(int offset) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            writePoint = offset;
            fseek(file, sizeof(preface), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);
        }

    public:
    	MemoryPool()=delete;
        explicit MemoryPool(const string &_filename, const preface &pre = preface{}, int _capacity = 120)
                : filename(_filename), cache(_capacity), hashmap() {
            file = fopen(filename.c_str(), "rb");
            if (file == NULL) {
            	std::cout<<"NULL\n"; 
                file = fopen(filename.c_str(), "wb+");
                fclose(file);
                writePoint = -1;
                preface tmp(pre);
                file = fopen(filename.c_str(), "rb+");
                fseek(file, 0, SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&tmp), sizeof(preface), 1, file);
                fseek(file, sizeof(preface), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
                fclose(file);
            } else {
                fseek(file, sizeof(preface), SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fclose(file);
            }
        }

        ~MemoryPool() {
            node_t *now = cache.head->next;
            while (now != cache.tail) {
                if (now->useless)update_File(now->key, *now->value);
                now = now->next;
            }
        }

        T read(int offset) {
            T tmp = count_Cache(offset) ? *hashmap[offset]->value : read_File(offset);
            write_Cache(offset, tmp);
            return tmp;
        }
    	T get(int offset){return read(offset);}

        int write(const T &o) {
            int offset = write_File(o);
            write_Cache(offset, o);
            return offset;
        }
        int add(const T &o){return write(o);} 
        
        void update(const T &o, int offset) {
            write_Cache(offset, o);
            hashmap[offset]->useless = true;
        }
  		void update(int offset,const T &o){update(o,offset);}

        void erase(int offset) {
            if (count_Cache(offset))erase_Cache(offset);
            erase_File(offset);
        }
    	void remove(int offset){erase(offset);}
    	
        void clear(preface ex = preface{}) {
            hashmap.clear();
            cache.clear();
            file = fopen(filename.c_str(), "wb+");
            fclose(file);
            writePoint = -1;
            preface tmp(ex);
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&tmp), sizeof(preface), 1, file);
            fseek(file, sizeof(preface), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);
        }
    	void clearAll(){clear();}

        preface readPre() {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            preface tmp;
            fread(reinterpret_cast<char *>(&tmp), sizeof(preface), 1, file);
            fclose(file);
            return tmp;
        }
    	preface readExtraBlock(){return readPre();}

        void updatePre(const preface &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(preface), 1, file);
            fclose(file);
        }
    	void writeExtraBlock(const preface& o){updatePre(o);}

        int getWritePoint() {
            if (writePoint >= 0)return writePoint;
            else {
                file = fopen(filename.c_str(), "rb+");
                fseek(file, 0, SEEK_END);
                int tmpWritePoint = ftell(file);
                fclose(file);
                return tmpWritePoint;
            }
        }
    };
}
#endif //BPLUSTREE_LRUMEMORYPOOL_HPP
