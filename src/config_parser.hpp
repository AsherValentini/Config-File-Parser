#pragma once

#include "reuse_pool_allocator.hpp"

#include <string>
#include <map>

class ConfigParser {

public:
    // Default constructor (RoF)
    ConfigParser();

    // Default destructor (RoF)
    ~ConfigParser();

    //TODO make the reuse pool allocator moveable and copyable so that we can perform the RoF

    void parseFromFile(const std::string& filepath); // its important to not make a copy or change the original data in that memory location hence const and lvalue_reference arg passing
    std::string getValue(const std::string& key) const; // while retrieving a key from the config we should not change or make a copy hence const std::key&

private:
    using MapType = std::map<
        std::string, std::string,
        std::less<>,
        ReusePoolAllocator<std::pair<const std::string, std::string>>
    >;
    MapType config_;
};
