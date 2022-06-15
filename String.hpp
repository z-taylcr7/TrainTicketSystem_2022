//
// Created by Cristiano on 2022/5/23.
//

#ifndef BPLUSTREE_STRING_HPP
#define BPLUSTREE_STRING_HPP
#include <cstring>
#include <iostream>
using istream=std::istream;
using ostream=std::ostream;
class String{
    friend istream &operator >>(istream &is, String &x){
        is >> x.str;
        return is;
    }

    friend ostream &operator <<(ostream &os, const String &x){
        for (const char *i = x.str; *i; ++i)
            os << *i;
        return os;
    }

private:
    char str[32];

public:
    String() { str[0] = '\0'; }

    explicit String(const char &c) { str[0] = c; str[1] = '\0'; }

    explicit String(const std::string &str_){
        int n = str_.length();
        for (int i = 0; i < n; ++i)
            str[i] = str_[i];
        str[n] = '\0';
    }

    explicit String(const char *&&str_){
        str[0] = '\0';
        strcpy(str, str_);
    }

    String &operator=(const char *&&str_){
        str[0] = '\0';
        strcpy(str, str_);
        return *this;
    }

    String(const String &other){
        str[0] = '\0';
        strcpy(str, other.str);
    }

    String &operator=(const String &other){
        if (&other == this) return *this;
        str[0] = '\0';
        strcpy(str, other.str);
        return *this;
    }

    String operator+(const String &other){
        char ret[36];
        ret[0] = '\0';
        strcat(ret, str), strcat(ret, other.str);
        return String(ret);
    }

    String &operator+=(const String &other){
        return *this = *this + other;
    }

    explicit operator std::string () const{
        return std::string(str) ;
    }

    bool operator==(const String &other) const{
        if (strlen(str) != strlen(other.str)) return false;
        for (const char *i = str, *j = other.str; *i && *j; ++i, ++j)
            if (*i != *j) return false;
        return true;
    }

    bool operator!=(const String &other) const{
        return !(*this == other);
    }

    bool operator<(const String &other) const{
        const char *i = str, *j = other.str;
        for (; *i && *j; ++i, ++j)
            if (*i != *j) return *i < *j;
        return !*i && *j;
    }

    bool operator<=(const String &other) const{
        return *this < other || *this == other;
    }

    bool operator>(const String &other) const{
        return other < *this;
    }

    bool operator>=(const String &other) const{
        return *this > other || *this == other;
    }

    char &operator[](int offset){ return str[offset]; }

    const char &operator[](int offset) const{ return str[offset]; }

    bool empty() const{ return !*str; }

    void clear(){ str[0] = '\0'; }
};
#endif //BPLUSTREE_STRING_HPP
