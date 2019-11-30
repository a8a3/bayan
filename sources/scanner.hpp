#pragma once

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "options.hpp"
#include "filters.hpp"
#include "hash.hpp"

namespace bayan {

// ------------------------------------------------------------------
class files_scanner {
   
public:
   // ---------------------------------------------------------------
   files_scanner(const bayan::options& opt) : options_(opt) {
      set_filters();
   };

   // ---------------------------------------------------------------
   ~files_scanner() = default;

   // ---------------------------------------------------------------
   void scan() const {
      const auto& input_dirs = options_.in_dirs_;

      if (input_dirs.empty()) {
         return;
      }

      const auto files_to_scan = get_files_to_scan();

      if (files_to_scan.empty()) {
         return;
      }

      do_scan(files_to_scan);
   }

   // ---------------------------------------------------------------
   std::vector<std::string> get_files_to_scan() const {
      namespace fs = boost::filesystem;
      std::vector<std::string> files;

      try {
         for (const auto &dir: options_.in_dirs_) {
            if(!fs::exists(dir) || !fs::is_directory(dir)) {
               continue; // for
            }

            fs::recursive_directory_iterator rdi{dir}, end;

            while (rdi != end) {
               if (filter_->apply(rdi)) {
                  files.push_back(rdi->path().string());
               }

               boost::system::error_code ec;
               rdi.increment(ec);

               if (ec) {
                  std::cout << "error accessing to: " << rdi->path().string() << ", " << ec.message() << std::endl;
               }
            }
         }
      }
      catch (const fs::filesystem_error& e) {
         std::cerr << e.what();
         throw;
      }
      return files;
   }

   using chunk_data = std::pair<std::string, bool>;
   // ---------------------------------------------------------------
   static chunk_data get_file_chunk(const std::string& file_name, size_t offset, size_t size) {
      std::string chunk(size, '\0');
      std::ifstream s(file_name, std::ifstream::binary);
      s.seekg(offset);
      s.read(&chunk[0], size);
      return {chunk, s.eof()};      
   }

private:
   // ---------------------------------------------------------------
   void do_scan(const std::vector<std::string>& files) const{
      basket b;
      std::copy(files.begin(), files.end(), std::back_inserter(b.files));
      const auto hash_algo = get_hash_algorithm(options_.conv_algo_);
      build_files_baskets(b, 0, hash_algo);
   }

   // ---------------------------------------------------------------
   void set_filters() {
      filter_ = std::make_unique<file_size_filter>(options_.min_file_sz_ ? options_.min_file_sz_: 1);

      if (options_.files_masks_.size()) {
         filter_->set_next_filter(std::make_unique<file_name_filter>(options_.files_masks_));
      }

      if (options_.recursive_ == 0 || options_.excl_dirs_.size()) {
         filter_->set_next_filter(std::make_unique<directories_filter>(options_.excl_dirs_, options_.recursive_));
      }
   }

   // ---------------------------------------------------------------
   struct basket {
      size_t index{0};
      std::list<std::string>   files;
      std::map<size_t, basket> childs;
      bool is_last{false};
   };

   // ---------------------------------------------------------------
   void build_files_baskets(basket& b, size_t chunk_ind, hash_algorithm hash_algo) const{

      if (b.is_last && b.files.size() > 1) {        
         std::cout << "the same files are: " << std::endl;
      
         for (const auto& file: b.files) {
               std::cout << file << std::endl;
         }
         std::cout << std::endl;
         return;
      }

      for (const auto& file : b.files) {
         const auto chunk = get_file_chunk(file, chunk_ind*options_.block_sz_, options_.block_sz_);
         const auto hash = get_hash(chunk.first, hash_algo);
      
         auto& child = b.childs[hash];
         child.files.push_back(file);
         child.is_last = chunk.second;
      }

      for (auto& child : b.childs) {
         auto& child_basket = child.second;
         if (child_basket.files.size() > 1) {
            build_files_baskets(child_basket, ++chunk_ind, hash_algo);
         }
      }
   } 

   const bayan::options& options_;
   filter::filter_ptr filter_;
}; 

} // namespace bayan