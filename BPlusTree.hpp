//
// Created by Cristiano on 2022/5/11.
//
#include "utility.hpp"
#include "String.hpp"
#include "STL.h"
#include "LRUMemoryPool.hpp"
#ifndef BPLUSTREE_BPLUSTREE_HPP
#define BPLUSTREE_BPLUSTREE_HPP
namespace Geneva{

    template<int M = 200,

//    (4096 - 5 * sizeof(int) - sizeof(bool)) / (sizeof(String)+sizeof(long long)+ sizeof(int)) - 1,
            int L = 200,
            //          (4096 - 4 * sizeof(int)) / (sizeof(String)+sizeof(long long)+ sizeof(int)) - 1,
            int CACHESIZE = 30>
    class BPlusTree {
        ///Announcement:
        ///go left: <key
        ///go right: >=key
        using Key=std::pair<unsigned long long,long long>;
        using data=int;

        class leafNode;

        class innerNode;

        enum sizeInfo {
            PAGESIZE = 4096,
            MAX_RECORD_NUM = L + 1,
            MIN_RECORD_NUM = (L+1) / 2,//   [ Min , Max )
            MAX_KEY_NUM = M + 1,
            MIN_KEY_NUM = (M - 1) / 2,
            MAX_CHILD_NUM = M+2
        };
        struct preface {
            int root = -1;
            int head = -1;
            int size = 0;
            preface()=default;
            preface(const int&_root,const int&_head,const int&_size):root(_root),head(_head),size(_size){}

        };
        struct splitReturn {
            Key key;int offset=-1;
            splitReturn()=default;
            splitReturn(const Key&_key,const int&_oo):key(_key),offset(_oo){}
        };

        struct removeReturn {
            bool completed;
            bool remakeParent;

        };
        struct insertReturn {
            bool childIncreased= false;
            int offset=-1;
            Key key;
        };

        MemoryPool<leafNode, preface>* memoLeaf;
        MemoryPool<innerNode, preface>* memoInner;
        preface basicInfo;
        innerNode rootNode;

        class leafNode {
        public:
            Key leafKey[MAX_RECORD_NUM];
            data leafData[MAX_RECORD_NUM]={0};
            int offset = -1;
            int leftBro = -1, rightBro = -1;
            int sum = 0;

            void addElement(BPlusTree *Tree, const Key &k, const data &val) {
                int pos = upper_bound(leafKey, leafKey + sum, k);
                for (int i = sum - 1; i >= pos; i--) {
                    leafKey[i + 1] = leafKey[i];
                    leafData[i + 1] = leafData[i];
                }
 //               if(tim==8066)std::cout<<k.first<<' '<<std::endl;
                leafKey[pos] = k;
                leafData[pos] = val;
                sum++;
                Tree->memoLeaf->update( *this,offset);
            }

            bool deleteElement(BPlusTree *Tree, const Key &k) {
                int pos = lower_bound(leafKey, leafKey + sum, k);
                if (pos == sum)return false;
                if (leafKey[pos] != k)return false;
                for (int i = pos ; i < sum - 1; i++) {
                    leafKey[i] = leafKey[i + 1];
                    leafData[i] = leafData[i + 1];
                }
                sum--;
                Tree->memoLeaf->update( *this,offset);
                return true;
            }

            bool updateElement(BPlusTree *Tree, const Key &k, const data &val) {
                int lpos = lower_bound(leafKey, leafKey + sum, k);
                int rpos = upper_bound(leafKey, leafKey + sum, k);
                bool ans = false;
                for (int i = lpos; i <= rpos; i++) {
                    if (leafKey[i] == k) {
                        leafData = val;
                        ans = true;
                    }
                }
                Tree->memoLeaf->update( *this,offset);
                return ans;
            }

            splitReturn splitNode(BPlusTree *Tree) {
                leafNode novel;
                splitReturn res;
                novel.leftBro = offset;
                novel.rightBro = rightBro;
                novel.offset=Tree->memoLeaf->getWritePoint();
                if (rightBro>-1) {
                    leafNode tmp = Tree->memoLeaf->read(rightBro);
                    tmp.leftBro = novel.offset;
                    Tree->memoLeaf->update(tmp, tmp.offset);
                }
                rightBro=novel.offset;
                novel.sum = MAX_RECORD_NUM-MIN_RECORD_NUM;
                sum = MIN_RECORD_NUM;
                for (int i = 0, j = sum; i < novel.sum; i++, j++) {
                    novel.leafKey[i] = leafKey[j];
                    novel.leafData[i] = leafData[j];
                }
                res.key = novel.leafKey[0];
                res.offset = Tree->memoLeaf->write(novel);
                Tree->memoLeaf->update(*this,offset);
                return res;
            }

            //delete
            void borrowLeft(BPlusTree *Tree, leafNode &left_bro, innerNode &father,
                            int pos) {//the posth son borrows from the (pos-1)th
                for (int i = sum; i > 0; i--) {
                    leafKey[i] = leafKey[i - 1];
                    leafData[i] = leafData[i - 1];
                }
                leafKey[0] = left_bro.leafKey[left_bro.sum - 1];
                leafData[0] = left_bro.leafData[left_bro.sum - 1];
                left_bro.sum--;
                sum++;
                father.nodeKey[pos - 1] = leafKey[0];
                Tree->memoInner->update(father, father.offset);
                Tree->memoLeaf->update(*this,offset);
                Tree->memoLeaf->update(left_bro, left_bro.offset);
            }

            void borrowRight(BPlusTree *Tree, leafNode &right_bro, innerNode &father,
                             int pos) {//the pos-1 th son borrows from the pos th
                leafKey[sum] = right_bro.leafKey[0];
                leafData[sum] = right_bro.leafData[0];
                for (int i = 0; i < right_bro.sum - 1; i++) {
                    right_bro.leafKey[i] = right_bro.leafKey[i + 1];
                    right_bro.leafData[i] = right_bro.leafData[i + 1];
                }
                right_bro.sum--;
                sum++;
                father.nodeKey[pos] = right_bro.leafKey[0];
                Tree->memoInner->update(father, father.offset);
                Tree->memoLeaf->update(*this,offset);
                Tree->memoLeaf->update(right_bro, right_bro.offset);
            }

            //always delete right,reserve left
            void mergeLeft(BPlusTree *Tree, leafNode &left_bro, innerNode &father) {

                left_bro.rightBro = rightBro;
                if (left_bro.rightBro>=0) {
                    leafNode tmp = Tree->memoLeaf->read(left_bro.rightBro);
                    tmp.leftBro = left_bro.offset;
                    Tree->memoLeaf->update(tmp, tmp.offset);
                }
                father.sum--;//delete the rightest one
                int sum2 = left_bro.sum;
                for (int i = sum - 1; i >= 0; i--) {
                    left_bro.leafKey[i + sum2] = leafKey[i];
                    left_bro.leafData[i + sum2] = leafData[i];
                }
                left_bro.sum += sum;
                Tree->memoInner->update(father, father.offset);
                Tree->memoLeaf->update(left_bro, left_bro.offset);
                Tree->memoLeaf->erase(offset);

            }

            void mergeRight(BPlusTree *Tree, leafNode &right_bro, innerNode &father, int index) {
                rightBro = right_bro.rightBro;
                if (right_bro.rightBro>=0) {
                    leafNode tmp = Tree->memoLeaf->read(right_bro.rightBro);
                    tmp.leftBro = offset;
                    Tree->memoLeaf->update( tmp,tmp.offset);
                }
                //delete nodekey[index] and pointer[index+1]
                for (int i = index; i < father.sum - 1; i++) {
                    father.nodeKey[i] = father.nodeKey[i + 1];
                    father.Pointer[i + 1] = father.Pointer[i + 2];
                }
                father.sum--;
                int sum2 = right_bro.sum;
                for (int i = sum2 - 1; i >= 0; i--) {
                    leafKey[i + sum] = right_bro.leafKey[i];
                    leafData[i + sum] = right_bro.leafData[i];
                }
                sum += right_bro.sum;

                Tree->memoInner->update(father, father.offset);
                Tree->memoLeaf->erase(right_bro.offset);
                Tree->memoLeaf->update(*this,offset);

            };

            //if father need to resize,return true
            bool resize(BPlusTree *Tree, innerNode &father, int index) {
                if (sum < MIN_RECORD_NUM) {
                    if (index == 0 && rightBro >= 0) {
                        leafNode right = Tree->memoLeaf->read(rightBro);
                        if (right.sum <= MIN_RECORD_NUM) {
                            mergeRight(Tree, right, father, index);
                            return true;
                        } else {
                            borrowRight(Tree, right, father, index);
                            return false;
                        }
                    } else if (index == father.sum && leftBro >= 0) {

                        leafNode left = Tree->memoLeaf->read(leftBro);
                        if (left.sum <= MIN_RECORD_NUM) {
                            mergeLeft(Tree, left, father);
                            return true;
                        } else {
                            borrowLeft(Tree, left, father, index);
                            return false;
                        }
                    } else if (leftBro >= 0 && rightBro >= 0) {
                        leafNode left = Tree->memoLeaf->read(leftBro);
                        if (left.sum > MIN_RECORD_NUM) {
                            borrowLeft(Tree, left, father, index);
                            return false;
                        } else {
                            leafNode right = Tree->memoLeaf->read(rightBro);
                            if (right.sum > MIN_RECORD_NUM) {
                                borrowRight(Tree, right, father, index);
                                return false;
                            } else {
                                mergeRight(Tree, right, father, index);
                                return true;
                            }
                        }
                    } else return false;
                } else return false;
            }
            void show() const {
                std::cout << "[leafNode]" << std::endl;
                std::cout << "offset: " << offset << std::endl;
                std::cout << "leftBro: " << leftBro << std::endl;
                std::cout << "rightBro: " << rightBro << std::endl;
                std::cout << "sum: " << sum << std::endl;
                std::cout << "leafKey & leafData:" << std::endl;
                for (int i = 0; i < sum; i++) {
                    std::cout << "leafKey: " <<leafKey[i].first<<' '<<leafKey[i].second << "\t\t\t\t\t\t\t\t\t\t\t" << "leafData: " << leafData[i] << std::endl;
                }
                std::cout << std::endl;
            }
        };

        class innerNode {
        public:
            int leftBro = -1;
            int rightBro = -1;
            Key nodeKey[MAX_KEY_NUM];
            int Pointer[MAX_KEY_NUM+1] = {0};
            int offset = -1;
            int sum = 0;//number of nodeKeys

            bool childIsLeaf = false;

            void addElement(BPlusTree *Tree, const splitReturn &sr, int pos) {
                for (int i = sum - 1; i >= pos; i--) {
                    nodeKey[i + 1] = nodeKey[i];
                    Pointer[i + 2] = Pointer[i + 1];
                }
                nodeKey[pos] = sr.key;
                Pointer[pos + 1] = sr.offset;
                sum++;
                Tree->memoInner->update(*this,offset);
            }

            void splitRoot(BPlusTree *Tree) {
                innerNode novel, newroot;
                newroot.offset = Tree->memoInner->write(newroot);
                novel.offset = rightBro = Tree->memoInner->getWritePoint();
                novel.leftBro = offset;
                novel.childIsLeaf = childIsLeaf;
                for (int i = MIN_KEY_NUM + 1, j = 0; i < sum; j++, i++) {
                    novel.nodeKey[j] = nodeKey[i];
                    novel.Pointer[j] = Pointer[i];
                }
                novel.Pointer[sum - MIN_KEY_NUM - 1] = Pointer[sum];
                novel.sum = sum - MIN_KEY_NUM - 1;
                sum = MIN_KEY_NUM;
                newroot.Pointer[0] = offset;
                newroot.Pointer[1] = novel.offset;
                newroot.nodeKey[0] = nodeKey[MIN_KEY_NUM];
                newroot.sum = 1;
                Tree->memoInner->write(novel);
                Tree->memoInner->update(newroot, newroot.offset);
                Tree->memoInner->update(*this,offset);
                Tree->basicInfo.root = newroot.offset;
                Tree->memoLeaf->updatePre(Tree->basicInfo);
                Tree->rootNode = newroot;
   //             std::cout<<"splitroot here"<<std::endl;
            }

            splitReturn splitNode(BPlusTree *Tree) {
                innerNode novel;
                novel.leftBro = offset;
                novel.rightBro=rightBro;
                novel.childIsLeaf = childIsLeaf;
                novel.offset = Tree->memoInner->getWritePoint();
                if (rightBro>=0) {
                    innerNode tmp = Tree->memoInner->read(rightBro);
                    tmp.leftBro = novel.offset;
                    Tree->memoInner->update(tmp,tmp.offset);
                }
                rightBro = novel.offset;
                for (int i = MIN_KEY_NUM + 1, j = 0; i < sum; j++, i++) {
                    novel.nodeKey[j] = nodeKey[i];
                    novel.Pointer[j] = Pointer[i];
                }
                novel.Pointer[sum - MIN_KEY_NUM - 1] = Pointer[sum];
                novel.sum = sum - MIN_KEY_NUM - 1;
                sum = MIN_KEY_NUM;
                Tree->memoInner->write(novel);
                Tree->memoInner->update(*this,offset);
                splitReturn x;
                x.offset = novel.offset;
                x.key = nodeKey[MIN_KEY_NUM];
                return x;
            }

            void borrowLeft(BPlusTree *Tree, innerNode &left_bro, innerNode &father, int index) {
                Pointer[sum+1]=Pointer[sum];
                for (int i = sum; i > 0; i--) {
                    nodeKey[i] = nodeKey[i - 1];
                    Pointer[i] = Pointer[i - 1];
                }
                nodeKey[0] = father.nodeKey[index-1];
                Pointer[0] = left_bro.Pointer[left_bro.sum];
                father.nodeKey[index-1] = left_bro.nodeKey[left_bro.sum - 1];
                left_bro.sum--;
                sum++;
                Tree->memoInner->update(father, father.offset);
                Tree->memoInner->update(left_bro, left_bro.offset);
                Tree->memoInner->update(*this,offset);
            }

            void borrowRight(BPlusTree *Tree, innerNode &right_bro, innerNode &father, int index) {
                Pointer[sum + 1] = right_bro.Pointer[0];
                nodeKey[sum] = father.nodeKey[index];
                father.nodeKey[index] = right_bro.nodeKey[0];
                for (int i = 0; i < right_bro.sum-1; i++) {
                    right_bro.nodeKey[i] = right_bro.nodeKey[i + 1];
                    right_bro.Pointer[i] = right_bro.Pointer[i + 1];
                }
                right_bro.Pointer[right_bro.sum-1] = right_bro.Pointer[right_bro.sum];
                right_bro.sum--;
                sum++;
                Tree->memoInner->update(father, father.offset);
                Tree->memoInner->update(right_bro, right_bro.offset);
                Tree->memoInner->update(*this,offset);
            }

            void mergeLeft(BPlusTree *Tree, innerNode &left_bro, innerNode &father) {
                //todo: debug
                left_bro.rightBro = rightBro;
                if (rightBro>=0) {
                    innerNode tmp = Tree->memoInner->read(rightBro);
                    tmp.leftBro = leftBro;
                    Tree->memoInner->update(tmp,tmp.offset);
                }
                left_bro.nodeKey[left_bro.sum] = father.nodeKey[father.sum-1];
                father.sum--;
                for (int i = 0, j = left_bro.sum + 1; i < sum; i++, j++) {
                    left_bro.nodeKey[j] = nodeKey[i];
                    left_bro.Pointer[j] = Pointer[i];
                }
                left_bro.sum += sum + 1;
                left_bro.Pointer[left_bro.sum] = Pointer[sum];
                Tree->memoInner->update(father, father.offset);
                Tree->memoInner->update(left_bro, left_bro.offset);
                Tree->memoInner->erase(offset);
            }

            void mergeRight(BPlusTree *Tree, innerNode &right_bro, innerNode &father, int index) {
                rightBro = right_bro.rightBro;
                if (rightBro>=0) {
                    innerNode tmp = Tree->memoInner->read(right_bro.rightBro);
                    tmp.leftBro = offset;
                    Tree->memoInner->update(tmp,tmp.offset);
                }
                nodeKey[sum] = father.nodeKey[index];
                for (int i = 0, j = sum + 1; i < right_bro.sum; i++, j++) {
                    nodeKey[j] = right_bro.nodeKey[i];
                    Pointer[j] = right_bro.Pointer[i];
                }

                Pointer[sum +right_bro.sum+ 1] =right_bro. Pointer[right_bro.sum];
                sum += right_bro.sum + 1;
                for (int i = index, j = index + 1; i < father.sum-1; i++, j++) {
                    father.nodeKey[i] = father.nodeKey[i + 1];
                    father.Pointer[j] = father.Pointer[j + 1];
                }
                father.sum--;
                Tree->memoInner->update(father, father.offset);
                Tree->memoInner->update(*this,offset);
                Tree->memoInner->erase(right_bro.offset);
            }

            void shortenRoot(BPlusTree *Tree) {
                if ((!childIsLeaf) && (sum <= 0)) {
                    Tree->basicInfo.root = Pointer[0];
                    Tree->memoLeaf->updatePre(Tree->basicInfo);
                    Tree->memoInner->erase(offset);
                    innerNode tmp = Tree->memoInner->read(Pointer[0]);
                   Tree->memoInner->update(tmp, tmp.offset);
                }
            }

            //if father need to resize,return true
            bool resize(BPlusTree *Tree, innerNode &father, int index) {
                if (sum < MIN_KEY_NUM) {
                    if (index == 0 && rightBro >= 0) {
                        innerNode right = Tree->memoInner->read(rightBro);
                        if (right.sum <= MIN_KEY_NUM) {
                            mergeRight(Tree, right, father, index);
                            return true;
                        } else {
                            borrowRight(Tree, right, father, index);
                            return false;
                        }
                    } else if (index == father.sum && leftBro >= 0) {

                        innerNode left = Tree->memoInner->read(leftBro);
                        if (left.sum <= MIN_KEY_NUM) {
                            mergeLeft(Tree, left, father);
                            return true;
                        } else {
                            borrowLeft(Tree, left, father, index);
                            return false;
                        }
                    } else if (leftBro >= 0 && rightBro >= 0) {
                        innerNode left = Tree->memoInner->read(leftBro);
                        if (left.sum > MIN_KEY_NUM) {
                            borrowLeft(Tree, left, father, index);
                            return false;
                        } else {
                            innerNode right = Tree->memoInner->read(rightBro);
                            if (right.sum > MIN_KEY_NUM) {
                                borrowRight(Tree, right, father, index);
                                return false;
                            } else {
                                mergeRight(Tree, right, father, index);
                                return true;
                            }
                        }
                    } else return false;
                } else return false;
            }
            void show() const {
                std::cout << "[innerNode]" << std::endl;
                std::cout << "offset: " << offset << std::endl;
                std::cout << "leftBro: " << leftBro << std::endl;
                std::cout << "rightBro: " << rightBro << std::endl;
                std::cout << (childIsLeaf ? "child node is leaf" : "child node is inner") << std::endl;
                std::cout << "sum: " << sum << std::endl;
                std::cout << "nodeKey & Pointer:" << std::endl;
                for (int i = 0; i < sum; i++) {
                    std::cout << "\t\t\t\t\t\t\t\t\t\t\t" << "Pointer: " << Pointer[i] << std::endl;
                    std::cout << "nodeKey: " << nodeKey[i].first<<' '<<nodeKey[i].second << std::endl;
                }
                std::cout << "\t\t\t\t\t\t\t\t\t\t\t" << "Pointer: " << Pointer[sum] << std::endl;
                std::cout << std::endl;

            }
        };

    private:
        void initialize(const Key &key, const data &val) {

//        	puts("BPlusTree initialize here");
            rootNode.offset = memoInner->getWritePoint();
//            puts("1");
            rootNode.childIsLeaf = true;
            rootNode.Pointer[0] = memoLeaf->getWritePoint();
//            puts("2");
            memoInner->write(rootNode);
//            puts("3");
            leafNode tempNode;
            tempNode.offset = rootNode.Pointer[0];
            tempNode.sum = 1;
//            std::cout<<"init "<<key.first<<' '<<key.second<<std::endl;
            tempNode.leafKey[0] = key;
            tempNode.leafData[0] = val;
//            tempNode.show();
            memoLeaf->write(tempNode);
//            puts("4");
            basicInfo.head = tempNode.offset;
            basicInfo.root = rootNode.offset;
            memoLeaf->updatePre(basicInfo);
//            if(tim==8066)show();
        }

        insertReturn BPInsert(int cur, const Key &k, const data &v) {
//            std::cout<<"BPInserting here"<<std::endl;

            innerNode curNode(memoInner->read(cur));
            int index = Geneva::upper_bound(curNode.nodeKey, curNode.nodeKey + curNode.sum, k);
            if (curNode.childIsLeaf) {
                leafNode toAdd (memoLeaf->read(curNode.Pointer[index]) );
                toAdd.addElement(this, k, v);
                insertReturn ir;
                if (toAdd.sum == MAX_RECORD_NUM) {
                    curNode.addElement(this, toAdd.splitNode(this), index);
                    if (curNode.sum == MAX_KEY_NUM) {
                        ir.childIncreased = true;
                        splitReturn sr = curNode.splitNode(this);
                        ir.offset = sr.offset, ir.key = sr.key;
                    } else ir.childIncreased = false;
                } else ir.childIncreased = false;
                return ir;
            } else {
                insertReturn ir{this->BPInsert(curNode.Pointer[index], k, v)};
                if (ir.childIncreased) {
                    curNode.addElement(this, {ir.key, ir.offset}, index);
                    if (curNode.sum == MAX_KEY_NUM) {
                        splitReturn sr = curNode.splitNode(this);
                        ir.offset = sr.offset, ir.key = sr.key;
                    } else ir.childIncreased = false;
                }
                return ir;
            }

        };

        removeReturn BPRemove(int cur, const Key &key) {
            innerNode curNode(memoInner->read(cur));
            int index = Geneva::upper_bound(curNode.nodeKey, curNode.nodeKey + curNode.sum, key);
            if (curNode.childIsLeaf) {
                leafNode toRemove{memoLeaf->read(curNode.Pointer[index])};
                removeReturn er;
                er.completed = toRemove.deleteElement(this, key);
                er.remakeParent = toRemove.resize(this, curNode, index);
                return er;
            } else {
                removeReturn er=BPRemove(curNode.Pointer[index], key) ;
                if (!er.remakeParent || !er.completed)return er;
                else {
                    innerNode son = memoInner->read(curNode.Pointer[index]);
                    er.remakeParent = son.resize(this, curNode, index);
                    return er;
                }

            }
        };

        void BPFind(int cur, const unsigned long long&key,vector<std::pair<int,long long>>&res) {
            //找一数列可行的

            Key defaultKey = std::make_pair(key, -9223372036854775808ll);
            innerNode curNode(memoInner->read(cur));
            int index = Geneva::upper_bound(curNode.nodeKey, curNode.nodeKey + curNode.sum, defaultKey);
            if (curNode.childIsLeaf) {
                leafNode target(memoLeaf->read(curNode.Pointer[index]));
                int pos = Geneva::lower_bound(target.leafKey, target.leafKey + target.sum, defaultKey);
                if (pos == target.sum) {
                    if (target.rightBro >= 0) {
                        target = memoLeaf->read(target.rightBro);
                        pos = Geneva::lower_bound(target.leafKey, target.leafKey + target.sum, defaultKey);
                    }
                    if (pos == target.sum)return;
                }
                if (target.leafKey[pos].first == key) {
                    while (target.leafKey[pos].first == key) {
                        res.push_back(std::make_pair(target.leafData[pos],target.leafKey[pos].second));
                        pos++;
                        if (pos == target.sum) {
                            if (target.rightBro >= 0) {
                                pos = 0;
                                target = memoLeaf->read(target.rightBro);
                            } else break;
                        }
                    }
                    return;
                } else return;
            } else {
                BPFind(curNode.Pointer[index], key, res);
                return;
            }
        };

        void BPUpdate(int cur, const Key &key, const data &val) {
            innerNode curNode(memoInner->read(cur));
            int index = Geneva::upper_bound(curNode.nodeKey, curNode.nodeKey + curNode.sum, key);
            if (curNode.childIsLeaf) {
                leafNode target(memoLeaf->read(curNode.Pointer[index]));
                if (!target.updateElement(this, key, val)) {
                    while (target.leftBro >= 0) {
                        target = memoLeaf->read(target.leftBro);
                        if (target.updateElement(this, key, val))break;
                    }
                }
            } else {
                BPUpdate(curNode.Pointer[index], key, val);
            }
        };

    public:
    	void printbacis(){
    		std::cout<<"basicInfo.root = "<<basicInfo.root<<std::endl;
    		std::cout<<"rootNode.offset = "<<rootNode.offset<<std::endl;
		}
        BPlusTree()=default;
        explicit BPlusTree(const std::string &fn) {
            memoLeaf=new MemoryPool<leafNode, preface>(fn + "'s leaves", (preface){-1,-1,0},CACHESIZE);
            memoInner=new MemoryPool<innerNode, preface>(fn + "'s inners", (preface){-1,-1,0},CACHESIZE);
            basicInfo=memoLeaf->readPre();
            rootNode=basicInfo.root == -1 ? innerNode{} : memoInner->read(basicInfo.root);

        }

        ~BPlusTree() {
            memoLeaf->updatePre(basicInfo);
            memoInner->updatePre(basicInfo);
            memoInner->update(rootNode, basicInfo.root);
            delete memoLeaf;
            delete memoInner;
        }

        int size() {
            return basicInfo.size;
        }

        int empty() {
            return basicInfo.size == 0;
        }

        void clear() {
            memoInner->clear();
            memoLeaf->clear();
            basicInfo = memoLeaf->readPre();
        }
        unsigned long long stringHash(const std::string& hash_in)
        {
            unsigned long long res=0;
            for(auto it:hash_in)res=(res<<16)+res+(unsigned int)it;
            return res;
        }

        void insert(const std::pair<string,long long>&_key, const data &val) {
            Key key={stringHash(_key.first),_key.second};
//        	puts("BPlusTree insert here");
 //           std::cout<<key.first;
            basicInfo.size++;
            memoLeaf->updatePre(basicInfo);

            if (basicInfo.root == -1){
                initialize(key, val);
                return;
            }
//            rootNode=memoInner->read(basicInfo.root);
//            std::cout<<"upper_bounding here"<<std::endl;
            int index = Geneva::upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.sum, key);
//            std::cout<<"upper_bounding finished"<<std::endl;
            if (rootNode.childIsLeaf) {
//                if(tim==8066)std::cout<<key.first<<' '<<std::endl;

//                std::cout<<"GG1 here"<<std::endl;
                leafNode lf = memoLeaf->read(rootNode.Pointer[index]);
//                std::cout<<"GG2 here"<<std::endl;
                lf.addElement(this, key, val);
//                std::cout<<"GG3 here"<<std::endl;
                if (lf.sum == MAX_RECORD_NUM){
                    rootNode.addElement(this,lf.splitNode(this),index);
                }
                if (rootNode.sum == MAX_KEY_NUM){
//                    std::cout<<"rootNode.splitRoot here"<<std::endl;
                    rootNode.splitRoot(this);
                }
            } else {
                insertReturn ir = BPInsert(rootNode.Pointer[index], key, val);
                if (ir.childIncreased) {
                    rootNode.addElement(this, {ir.key, ir.offset}, index);
                    if (rootNode.sum == MAX_KEY_NUM)rootNode.splitRoot(this);
                }
            }
 //           if(tim==8066)show();
        }

        bool remove(const Key &key) {
            if (basicInfo.size == 0 || basicInfo.root == -1)return false;
            bool deleted;
            int index = Geneva::upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.sum, key);
            if (rootNode.childIsLeaf) {
                leafNode target(memoLeaf->read(rootNode.Pointer[index]));
                deleted = target.deleteElement(this, key);
                if (deleted && target.resize(this, rootNode, index))rootNode.shortenRoot(this);
            }else{
                removeReturn er(BPRemove(rootNode.Pointer[index],key));
                if(!er.completed)return false;
                else{
                    deleted=true;
                    if(er.remakeParent){
                        innerNode son{memoInner->read(rootNode.Pointer[index])};
                        if(son.resize(this,rootNode,index))rootNode.shortenRoot(this);
                    }
                }
            }
            if(deleted){
                basicInfo.size--;
                memoLeaf->updatePre(basicInfo);


            }
            return deleted;
        }
        void find(const string&_key,vector<std::pair<int,long long>>&res){
            unsigned long long key=stringHash(_key);
            if(tim==603){
 //              std::cout<<key<<std::endl;show();
            }
            Key defaultKey= std::make_pair(key,-9223372036854775808ll);
            if (basicInfo.size == 0 || basicInfo.root == -1)return;
            rootNode=memoInner->read(basicInfo.root);
            int index;
            if(rootNode.sum==0){
                index=0;
            }else index = Geneva::upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.sum, defaultKey);

            if (rootNode.childIsLeaf) {
                leafNode target=memoLeaf->read(rootNode.Pointer[index]);
                int pos = Geneva::lower_bound(target.leafKey, target.leafKey + target.sum, defaultKey);
                if (pos == target.sum) {
                    if (target.rightBro >= 0) {
                        target = memoLeaf->read(target.rightBro);
                        pos = Geneva::lower_bound(target.leafKey, target.leafKey + target.sum, defaultKey);
                    }
                    if (pos == target.sum)return;
                }
                if (target.leafKey[pos].first == key) {
                    while(target.leafKey[pos].first == key&&pos<target.sum){
                        res.push_back(std::make_pair(target.leafData[pos],target.leafKey[pos].second));
                        pos++;
                        if(pos==target.sum&&(target.rightBro>=0)){
                            pos=0;target=memoLeaf->read(target.rightBro);
                        }
                    }
//                    if(res.size())std::cout<<"find out one:"<<res[0].second<<' '<<res[0].first<<std::endl;
 //                   else std::cout<<"find null"<<std::endl;
                    return;
                }
                else return;
            }
            else {
                BPFind(rootNode.Pointer[index], key,res);return;
            }
        }
        bool remove(const std::pair<string,long long>&_key,const int&id){
            Key key={stringHash(_key.first),_key.second};
            return remove(key);
        }
        bool containsKey(const Key&key){
            std::pair<data*,bool>fr=find(key);
            return fr.second;
        }
        void rollback(int ddl){
            memoLeaf->rollback(ddl);
            memoInner->rollback(ddl);
            basicInfo=memoLeaf->readPre();
           rootNode=memoInner->read(basicInfo.root);
        }

        /////debug
    private:
        void show(int offset, bool isLeaf) const {
            std::cout << "[pos] " << offset << std::endl;
            if (isLeaf) {
                leafNode tempNode = memoLeaf->read(offset);
                tempNode.show();
            }
            else {
                innerNode tempNode = memoInner->read(offset);
                tempNode.show();
                std::cout << std::endl;
                for (int i = 0; i <= tempNode.sum; i++) {
                    if (tempNode.childIsLeaf)show(tempNode.Pointer[i], true);
                    else show(tempNode.Pointer[i], false);
                }
            }
        };

    public:
        void show() const {
            std::cout << "[show]--------------------------------------------------------------------------------" << std::endl;
            show(basicInfo.root, false);
            std::cout << "[show]--------------------------------------------------------------------------------" << std::endl;
        }

        void showLeaves() const {
            int cur = basicInfo.head;
            while (cur >= 0) {
                leafNode nowNode = memoLeaf->read(cur);
                nowNode.show();
                cur = nowNode.rightBro;
            }
        }
    };

}

using namespace  Geneva;
#endif //BPLUSTREE_BPLUSTREE_HPP
