
#ifndef FOUREST_SEPARATOR_H
#define FOUREST_SEPARATOR_H


#include <iostream>
#include <string>
#include "STL.h"

namespace Fourest
{
    typedef std::string str;

    class separator
    {
        std::string source;
        vector<str> words;
        char ban_word;

        void process();
    public:
        separator()=default;
        explicit separator(const char &y):ban_word(y){}
        ~separator();

        int size() const;
        void clear();
        void process(const str &);
        bool readLine(std::istream &);
        vector<str> &content();

        str &operator[](int);
        bool empty() const {return words.empty();}
    };
}

namespace Fourest
{
    separator::~separator()
    {
        words.clear();
    }

    str &separator::operator[](int _pos)
    {
        return words[_pos];
    }

    int separator::size() const
    {
        return words.size();
    }

    void separator::process(const std::string &_str)
    {
        source=_str;
        process();
    }

    bool separator::readLine(std::istream &_is)
    {
        if(_is.eof()) return false;
        getline(_is,source);
        process();
        return true;
    }

    vector<str> &separator::content()
    {
        return words;
    }


    void separator::clear()
    {
        words.clear();
    }

    void separator::process()
    {
        words.clear();
        int startPos=0,Length=0; // For legal chars.
        int _startPos=0,Len=0; // For illegal chars.
        for(int i=0;i<source.size();++i)
        {
            if(ban_word==source[i])
            {
                if(Length>0) words.push_back(source.substr(startPos,Length));
                Length=0;
                ++Len;
            }
            else
            {
                if(Length==0) startPos=i;
                ++Length;
                Len=0;
            }
        }
        if(Length>0) words.push_back(source.substr(startPos,Length));
    }

}

#endif //FOUREST_SEPARATOR_H
