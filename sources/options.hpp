#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace bayan {
// ------------------------------------------------------------------
struct options {
   std::vector<std::string> in_dirs_;
   std::vector<std::string> excl_dirs_;
   std::vector<std::string> files_masks_;
   std::string              conv_algo_{"md5"};

   unsigned int min_file_sz_{1};
   unsigned int recursive_ {0};
   unsigned int block_sz_ {1};
};

// ------------------------------------------------------------------
std::ostream& operator << (std::ostream& os, const bayan::options& opt) {
   os << "scan parameters:\n";
   const auto print_str_list = [&os] (const auto head, const auto& list) {

      if (list.empty()) {
         os << head << " none\n";
         return;
      }
      os << head << '\n';
      for( const auto& item: list ) {
         os << item << '\n';
      }
   };

   print_str_list("include dirs: ", opt.in_dirs_);
   print_str_list("exclude dirs: ", opt.excl_dirs_);
   print_str_list("files masks:  ", opt.files_masks_);

   os << "conv algo:     " << opt.conv_algo_   << '\n';
   os << "recursive:     " << opt.recursive_   << '\n';
   os << "block size:    " << opt.block_sz_    << '\n';
   os << "min file size: " << opt.min_file_sz_ << '\n';

   return os;
}

} // namespace bayan