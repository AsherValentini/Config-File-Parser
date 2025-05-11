#pragma once

#include <string>
#include <map>

class ConfigParser {
public: 
    void parseFromFile(const std::string& filepath); // its important to not make a copy or change the orginal data in that memory location hence const and lvalue_reference arg passing
    std::string getValue(const std::string& key) const; // while retrieving a key from the config we should not change or make a copy hence const std::key&
private: 
    std::map<std::string, std::string> config_; 
};