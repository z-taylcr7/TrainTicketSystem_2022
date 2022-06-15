//
// Created by Cristiano on 2022/5/11.
//

#include <utility>
#include<iostream>

#ifndef BPLUSTREE_UTILITY_HPP
#define BPLUSTREE_UTILITY_HPP
namespace Geneva{
    template<class data>
    int upper_bound(data* st,data* ed,const data&key ){
        int mid,ans=0;
        int l=0,r=(ed-st);
//        std::cout<<l<<" "<<r<<std::endl; 
        while(r-l>0){
            mid=((r+l)>>1);
            if(key<st[mid]){
                r=mid;
            }
            else ans=mid+1,l=mid+1;
        }
        return ans;
    }
    template<class data>
    int lower_bound(data* st,data* ed,const data&key ){
        int mid;
        int ans=0;
        int r=(ed-st);
        int l=0;
        while(r-l>0){
            mid=((r+l)>>1);
            if( st[mid]<key){
                ans=mid+1;
                l=mid+1;
            }
            else r=mid;
        }
        return ans;
    }

}
#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#endif

#endif //BPLUSTREE_UTILITY_HPP
