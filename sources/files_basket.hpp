#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>

// ------------------------------------------------------------------
struct data {
    size_t index;
    std::list<std::string> files;
    std::map<size_t, data> childs;
};

using basket = std::map<size_t, data>;

// ------------------------------------------------------------------
void build_files_baskets( const std::vector<std::string>& files ) {
    basket b;
}