#pragma once

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "options.hpp"

namespace bayan {

// ------------------------------------------------------------------
class files_scanner {

public:

// TODO implement chain of resposibility

   files_scanner(const bayan::options& opt) : options_(opt) {};
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
//          std::cout << "do not check: " << rdi->path().string() << std::endl;
            rdi.no_push();
         }
         return false;
      }

      // TODO always check to non-empty files(size of file must be greater than 0)
      if (options_.min_file_sz_ > 1 && !check_file_size(rdi->path().string(), options_.min_file_sz_)) {
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

   const bayan::options& options_;
};

} // namespace bayan