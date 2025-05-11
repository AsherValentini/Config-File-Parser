#include <iostream>
#include "config_parser.hpp"

int main(){
    
    ConfigParser parser; 

    parser.parseFromFile("config.txt"); 

    std::cout << "Host: " << parser.getValue("host") << std::endl;  
    std::cout << "Port: " << parser.getValue("port") << std::endl; 

    return 0; 
}

