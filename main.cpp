#include "cmd.hpp"
#include <iostream>
#include <fstream>
#include "ErrorMessage.h"


//std::ifstream in("../data/basic_1/1.in");
//std::ofstream out("output.txt");



int main()
{
//    freopen("5.in","r",stdin);
//    freopen("1.ans","w",stdout);
	const bool is_online=1; 
    cmd ticketLibrary;
    if(is_online)
    {
        std::ios::sync_with_stdio(false);
        try {ticketLibrary.run(std::cin,std::cout);}
        catch(error x)
        {
            std::cout<<x.what()<<std::endl;
            return 0;
        }
    }
}
