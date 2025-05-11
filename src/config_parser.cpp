#include "config_parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>


void ConfigParser::parseFromFile(const std::string& filepath){

    //open the file safely 
    std::ifstream file(filepath); //we need the file at file path to be an input file stream (read only access as stream)
    if(!file){
        throw std::runtime_error("Cannot open file: " + filepath); 
    }

    //parse line by line
    std::string line; 

    while(std::getline(file, line)){

        //trim white spaces 
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end()); 

        //skip empty or comment lines 
        if(line.empty() || line[0] == '#') continue; 

        auto delimiterPos = line.find('='); 
        if(delimiterPos == std::string::npos) continue; 

        std::string key = line.substr(0, delimiterPos); 
        std::string value = line.substr(delimiterPos +1); 

        config_[key] = value; 

    } 
}

std::string ConfigParser::getValue(const std::string& key) const {
    
    auto it = config_.find(key); 
    if(it != config_.end()){
        return it->second; 
    }
    return {}; 
}