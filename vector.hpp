#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"
#include <climits>
#include <cstddef>

namespace Geneva
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector 
{
public:
    T** squad;
    size_t curSize;
    size_t maxSize;
    void clean(){
        for(int i=curSize-1;i>=0;i--){
            delete squad[i];
        }
        delete[] squad;
    }
    void reSpace(){
        T** newSquad=new T*[maxSize];
        for(int i=0;i<curSize;i++)newSquad[i]=new T((*this)[i]);
        clean();
        squad=newSquad;
    }
    void doubleSpace(){
    maxSize*=2;reSpace();
}
    void halfSpace(){
        if(2*curSize<=maxSize) {
            maxSize /= 2;reSpace();
        }
    }
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator 
	{
	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the 
	// iterator points to. 
	// STL algorithms and containers may use these type_traits (e.g. the following 
	// typedef) to work properly. In particular, without the following code, 
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
        friend class vector;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:

		 size_t ptr;
         vector<T> *vecHead;


	public:
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */

        iterator(vector<T> *vecHead,const int& ptr=0 ) :  vecHead(vecHead) ,ptr(ptr){}
		iterator operator+(const int &n) const 
		{
			//TODO
            iterator tmp=*this;
            tmp.ptr=tmp.ptr+n;
            return tmp;
		}


        iterator operator-(const int &n) const
		{
			//TODO
            iterator tmp=*this;
            tmp.ptr=tmp.ptr-n;
            return tmp;
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const 
		{
			//TODO
            iterator tmp=*this;
            if (vecHead != rhs.vecHead) throw (invalid_iterator){};
            int cnt=0;
            while(tmp.ptr--!=rhs.ptr){cnt++;}
            return cnt;
		}
		iterator& operator+=(const int &n) 
		{
			//TODO
            ptr=ptr+n;
            return *this;
		}
		iterator& operator-=(const int &n) 
		{
			//TODO

            ptr=ptr-n;
            return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
            iterator tmp=*this;
            ptr++;
            return tmp;
        }
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {ptr++;return *this;}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            iterator tmp=*this;
            ptr--;
            return tmp;}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {ptr--;return *this;}
		/**
		 * TODO *it
		 */
		T& operator*() const{return (*vecHead)[ptr];}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
            return (vecHead==rhs.vecHead&&(ptr==rhs.ptr));
        }
		bool operator==(const const_iterator &rhs) const {
            return (vecHead==rhs.vecHead&&(ptr==rhs.ptr));
        }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
		bool operator!=(const const_iterator &rhs) const {
            return !(*this==rhs);
        }
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */


	class const_iterator 
	{
	public:
        friend class vector;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;
	
	private:
		/*TODO*/
       const  vector<T>* vecHead;
        size_t ptr;
    public:
        const_iterator();
        const_iterator(const vector<T>* vecHead, const size_t& ptr=0) : vecHead(vecHead), ptr(ptr){}
        const_iterator operator+(const int &n) const
        {
            //TODO
            return const_iterator(vecHead,ptr+n);
        }
        const_iterator operator-(const int &n) const
        {
            //TODO
            return const_iterator(vecHead,ptr-n);
        }
        // return the distance between two const_iterators,
        // if these two const_iterators point to different vectors, throw invaild_const_iterator.
        int operator-(const const_iterator &rhs) const
        {
            //TODO
            const_iterator tmp=*this;
            if (vecHead != rhs.vecHead) throw (invalid_iterator){};
            int cnt=0;
            while(tmp.ptr--!=rhs.ptr){cnt++;}
            return cnt;
        }
        const_iterator& operator+=(const int &n)
        {
            //TODO
            ptr=ptr+n;
            return *this;
        }
        const_iterator& operator-=(const int &n)
        {
            //TODO

            ptr=ptr-n;
            return *this;
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const_iterator tmp=*this;
            ptr++;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {ptr++;return *this;}
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator tmp=*this;
            ptr--;
            return tmp;}
        /**
         * TODO --iter
         */
        const_iterator& operator--() {ptr--;return *this;}
        /**
         * TODO *it
         */
        const T& operator*() const{return (*vecHead)[ptr];}
        /**
         * a operator to check whether two const_iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return (vecHead==rhs.vecHead&&(ptr==rhs.ptr));
        }
        bool operator==(const const_iterator &rhs) const {
            return (vecHead==rhs.vecHead&&(ptr==rhs.ptr));
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this==rhs);
        }
    };


	/**
	 * TODO Constructs
	 * Atleast two: default constructor, copy constructor
	 */
	vector(int cur=0,int max=1):curSize(cur),maxSize(max) {
        squad=new T*[maxSize];
    }
	vector(const vector &other) :curSize(other.curSize),maxSize(other.maxSize),squad(new T*[other.maxSize]){
        for(int i=0;i<other.curSize;i++)squad[i]=new T(other[i]);
    }
	/**
	 * TODO Destructor
	 */
	~vector() {
        clean();
    }
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
        if(this==&other)return *this;
        clean();
        squad=new T*[other.maxSize];
        maxSize=other.maxSize;curSize=other.curSize;
        for(int i=0;i<other.curSize;i++)squad[i]=new T(other[i]);
        return *this;
    }
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
        if(pos<0||pos>=curSize)throw(index_out_of_bound){};
        return *squad[pos];

    }
	const T & at(const size_t &pos) const {
        if(pos<0||pos>=curSize)throw(index_out_of_bound){};
        return *squad[pos];
    }
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
        if(pos<0||pos>=curSize)throw(index_out_of_bound){};
        return *squad[pos];
    }
	const T & operator[](const size_t &pos) const {
        if(pos<0||pos>=curSize)throw(index_out_of_bound){};
        return *squad[pos];
    }
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
        if(curSize==0)throw(container_is_empty){};
        return *squad[0];
    }
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
        if(curSize==0)throw(container_is_empty){};
        return *squad[curSize-1];
    }
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
        return iterator(this,0);
    }
	const_iterator cbegin() const {
        return const_iterator(this,0);
    }
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
        return iterator(this,curSize);
    }
	const_iterator cend() const {
        return const_iterator(this,curSize);
    }
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
        return (curSize==0);
    }
	/**
	 * returns the number of elements
	 */
	size_t size() const {
        return curSize;
    }
	/**
	 * clears the contents
	 */
	void clear() {
        clean();
        curSize=0;maxSize=1;
        squad=new T*[maxSize];
    }
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
        return insert(pos.ptr,value);
    }
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
        if(ind<0||ind>curSize)throw index_out_of_bound{};
        if(curSize==maxSize)doubleSpace();
        T* tmp=new T(value);
        for(int i=curSize;i>ind;i--)squad[i]=squad[i-1];
        squad[ind]=tmp; curSize++;
        return iterator(this,ind);
    }
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
        return erase(pos.ptr);
    }
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
        if(ind<0||ind>=curSize)throw index_out_of_bound{};
        T* tmp=squad[ind];
        for(int i=ind;i<curSize-1;i++){
            squad[i]=squad[i+1];
        }
        squad[curSize-1]=tmp;
        delete squad[curSize-1];
        curSize--;
        if(curSize<maxSize/2)halfSpace();
        if(ind==curSize)return end();
        return iterator(this,ind);
    }
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
        if(curSize==maxSize)doubleSpace();
        squad[curSize]=new T(value);
        curSize++;
    }
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
        delete squad[curSize-1];
        curSize--;
        if(curSize<=maxSize/2)halfSpace();
    }
};

}

#endif
