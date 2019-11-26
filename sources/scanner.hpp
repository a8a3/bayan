#pragma once

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "options.hpp"
#include "filters.hpp"

namespace bayan {

// ------------------------------------------------------------------
class files_scanner {

public:
   files_scanner(const bayan::options& opt) : options_(opt) {
      set_filters();
   };

   ~files_scanner() = default;

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

   std::vector<std::string> get_all_files() const {
      return get_files_to_scan();
   }

private:
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
                  files.push_back(rdi->path().filename().string());
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

   void set_filters() {
      filter_ = std::make_unique<file_size_filter>(options_.min_file_sz_ ? options_.min_file_sz_: 1);

      if (options_.files_masks_.size()) {
         filter_->set_next_filter(std::make_unique<file_name_filter>(options_.files_masks_));
      }

      if (options_.recursive_ == 0 || options_.excl_dirs_.size()) {
         filter_->set_next_filter(std::make_unique<directories_filter>(options_.excl_dirs_, options_.recursive_));
      }
   }

// 0 1 2 3 4 5    0 2 4   1 3 5
// A A A A A A    A A A   A A A
// B D B D B D => B B B   D D D
// C E H F C F    C H C , E F F



   void do_scan(const std::vector<std::string>& files_to_scan) const {
      std::cout << "attempt to scan " << files_to_scan.size() << " files\n";

   }

   const bayan::options& options_;
   filter::filter_ptr filter_;
};

} // namespace bayan