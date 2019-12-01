#pragma once

#include <string>
#include <functional>

#include <boost/crc.hpp>
#include <boost/bimap.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

namespace {

// ------------------------------------------------------------------
size_t get_crc32_hash(const std::string& data) {
   boost::crc_32_type hash;
   hash.process_bytes(data.data(), data.size());
   return hash.checksum();
}

} // namespace

// ------------------------------------------------------------------
enum class hash_algorithm : int {
   std   = 0,
   crc32 = 1
};

using hashes = boost::bimap<std::string, int>;
const hashes dict = boost::assign::list_of<hashes::relation>
(std::string("std"),   0)
(std::string("crc32"), 1);

// ------------------------------------------------------------------
hash_algorithm get_hash_algorithm(const std::string& str_value) {
   const auto it = dict.left.find(str_value);

   if (it == dict.left.end()) {
      throw std::logic_error("wrong hash algorithm requested");
   }
   return static_cast<hash_algorithm>(it->second);
}

// ------------------------------------------------------------------
size_t get_hash(const std::string& data, hash_algorithm algo) {
   switch (algo) {
      case hash_algorithm::crc32 : return get_crc32_hash(data);
      case hash_algorithm::std   : return std::hash<std::string>{}(data);
   }
   throw std::logic_error("wrong hash algorithm requested");
}