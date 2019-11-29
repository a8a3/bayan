#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>

#include "hash.hpp"
#include "scanner.hpp"

// ------------------------------------------------------------------
struct basket {
    size_t index{0};
    std::list<std::string>   files;
    std::map<size_t, basket> childs;

    void sort() {
        constexpr auto size = 1;

        for(const auto& file: files) {
            const auto chunk = bayan::files_scanner::get_file_chunk(file, index*size, size);
            const auto hash = get_hash(chunk.first, hash_algorithm::std);

            auto& child = childs[hash];
            child.files.push_back(file);
        }
    }
};

// ------------------------------------------------------------------
void build_files_baskets( const std::vector<std::string>&) {
//    basket b;
//    std::copy(files.begin(), files.end(), std::back_inserter(b));



}