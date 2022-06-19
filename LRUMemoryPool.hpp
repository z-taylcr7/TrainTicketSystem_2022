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
//rollback:
// updatePre--0
// insert--1
// update--2
// erase--3
extern int tim;
namespace Geneva {
    template<class T, class preface = int>
    class MemoryPool {
    private:
        const string filename;
        const string logname;
        FILE *file;
        FILE *log;
        int writePoint=-1;
        int rbPoint=8;

        struct block{
            int offset;
            T t;
            int op;
            int time;
            preface pre;
        };


        int write_File_Rollback(const T &o) {
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
        int write_File(const T &o) {
            int offset;
            file = fopen(filename.c_str(), "rb+");
            log = fopen(logname.c_str(),"rb+");

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
            block b{offset,o,1,tim,preface()};
            fseek(log, rbPoint, SEEK_SET);
            fwrite(reinterpret_cast<const char*>(&b),sizeof(block),1,log);
            rbPoint+=sizeof (block);
            fclose(log);
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

        void update_File_Rollback(int offset, const T &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);

        }
        void update_File(int offset, const T &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            T old;
            fread(reinterpret_cast<char* >(&old),sizeof(T),1,file);

            log = fopen(logname.c_str(),"rb+");
            block b{offset,old,2,tim,preface()};
            fseek(log, rbPoint, SEEK_SET);

            fwrite(reinterpret_cast<const char*>(&b),sizeof(block),1,log);
            rbPoint+=sizeof (block);
            fclose(log);

            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);

        }

        void erase_File_Rollback(int offset) {
//        	std::cout<<"erase"<<std::endl; 
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            writePoint = offset;
            fseek(file, sizeof(preface), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);

        }
        void erase_File(int offset) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            T old;
            fread(reinterpret_cast<char*>(&old),sizeof(T),1,file);
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            writePoint = offset;
            fseek(file, sizeof(preface), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);

            log = fopen(logname.c_str(),"rb+");
            fseek(log, rbPoint, SEEK_SET);
            block b{offset,old,3,tim,preface()};
            fwrite(reinterpret_cast<const char*>(&b),sizeof(block),1,log);
            rbPoint+=sizeof (block);
            fclose(log);
        }

    public:
        MemoryPool()=delete;
        explicit MemoryPool(const string &_filename, const preface &pre = preface{}, int _capacity = 120)
                : filename(_filename), logname(_filename+"_rb") {
            file = fopen(filename.c_str(), "rb");
            log = fopen(logname.c_str(),"rb");
            if (file == NULL) {
                file = fopen(filename.c_str(), "wb+");
                log = fopen(logname.c_str(), "wb+");
                fclose(file);
                fclose(log);
                writePoint = -1;
                preface tmp(pre);
                file = fopen(filename.c_str(), "rb+");
                log = fopen(logname.c_str(), "rb+");
                fseek(file, 0, SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&tmp), sizeof(preface), 1, file);
                fseek(file, sizeof(preface), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);

                fseek(log,1,SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&rbPoint), sizeof(int), 1, log);
                fclose(file);
                fclose(log);
            } else {
                fseek(file, sizeof(preface), SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fseek(log,1,SEEK_SET);
                fread(reinterpret_cast< char *>(&rbPoint), sizeof(int), 1, log);
                //std::cout<<rbPoint<<std::endl;
                fclose(file);
                fclose(log);
            }
        }

        ~MemoryPool() {
            log = fopen(logname.c_str(),"rb+");
            fseek(log,1,SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&rbPoint), sizeof(int), 1, log);
            fseek(log,1,SEEK_SET);
            fread(reinterpret_cast< char *>(&rbPoint), sizeof(int), 1, log);
            fclose(log);
        }

        T read(int offset) {
            T tmp = read_File(offset);
            return tmp;
        }
        T get(int offset){return read(offset);}

        int write(const T &o) {
            int offset = write_File(o);
            return offset;
        }
        int add(const T &o){return write(o);}

        void update(const T &o, int offset) {
        	update_File(offset,o);
        }
        void update(int offset,const T &o){update(o,offset);}

        void erase(int offset) {
            erase_File(offset);
        }
        void remove(int offset){erase(offset);}

        void clear(preface ex = preface{}) {
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

        void updatePre_Rollback(const preface &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(preface), 1, file);
            fclose(file);
        }
        void updatePre(const preface &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            preface old;
            fread(reinterpret_cast<char *>(&old), sizeof(preface), 1, file);
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(preface), 1, file);
            fclose(file);

            log = fopen(logname.c_str(),"rb+");
            block b{0,T(),0,tim,old};
            fseek(log, rbPoint, SEEK_SET);
            fwrite(reinterpret_cast<const char*>(&b),sizeof(block),1,log);
            rbPoint+=sizeof (block);
            fclose(log);
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

        void rollback(int ddl){
            //log review
            log = fopen(logname.c_str(),"rb+");
            block b;
            while(rbPoint>8){
                rbPoint-=sizeof(block);
                fseek(log,rbPoint,SEEK_SET);
                fread(reinterpret_cast<char* >(&b),sizeof(block),1,log);
                if(b.time<ddl){
                    rbPoint+=sizeof(block);break;
                }
//            	std::cout<<"Time = "<<b.time<<std::endl;
                if(b.op==0){
                    updatePre_Rollback(b.pre);
                }
                if(b.op==1){
                    erase_File_Rollback(b.offset);
                }
                if(b.op==2){
                    update_File_Rollback(b.offset,b.t);
                }
                if(b.op==3){
                    write_File_Rollback(b.t);
                }
            }
            fclose(log);

        }
    };
}
#endif //BPLUSTREE_LRUMEMORYPOOL_HPP
