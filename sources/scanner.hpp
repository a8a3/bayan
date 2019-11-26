#pragma once

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "options.hpp"
#include "filters.hpp"

namespace bayan {

// ------------------------------------------------------------------
class files_scanner {

public:

// TODO implement chain of resposibility

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
               if (need_to_scan(rdi)) {
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

   // see: https://thispointer.com/c-get-the-list-of-all-files-in-a-given-directory-and-its-sub-directories-using-boost-c17/
   bool need_to_scan(boost::filesystem::recursive_directory_iterator& rdi) const {
      namespace fs = boost::filesystem;
      const auto& excl_dirs = options_.excl_dirs_;

      if (fs::is_directory(rdi->path())) {
         if (options_.recursive_ == 0 || std::find(excl_dirs.begin(), excl_dirs.end(), rdi->path().string()) != excl_dirs.end()) {
            rdi.no_push();
         }
         return false;
      }

      if (filter_ && !filter_->apply(rdi)) {
         return false;
      }
      return check_file_name_mask(rdi->path().filename().string());
   }

   bool check_file_name_mask(const std::string& file_name) const {
      const auto& filters = options_.files_masks_;

      const auto match_by_mask = [&file_name] (const auto& condition) {
         const boost::regex filter{condition};
         return boost::regex_match(file_name, filter);
      };
      return std::all_of(filters.begin(), filters.end(), match_by_mask);
   }

   bool check_file_size(const std::string& file_name, const size_t size) const {
      namespace fs = boost::filesystem;

      boost::system::error_code ec;
      const auto file_size = fs::file_size(file_name, ec);

      if (!ec) {
         return file_size >= size;
      }
      std::cout << "file '" << file_name << "' attribute reading error: " << ec.message() << std::endl;
      return false;
   }

   void set_filters() {
      filter_ = std::make_unique<file_size_filter>(options_.min_file_sz_ ? options_.min_file_sz_: 1); // min file size
   }

   const bayan::options& options_;
   filter::filter_ptr filter_;
};

} // namespace bayan