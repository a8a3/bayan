#pragma once

#include <boost/filesystem.hpp>

#include "options.hpp"

namespace bayan {

// ------------------------------------------------------------------
class files_scanner {

public:
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
   bool need_to_scan(const boost::filesystem::recursive_directory_iterator& rdi) const {
      namespace fs = boost::filesystem;
      const auto& excl_dirs = options_.excl_dirs_;

      if (fs::is_directory(rdi->path()) &&
         (options_.recursive_ == 0 || std::find(excl_dirs.begin(), excl_dirs.end(), rdi->path().filename()) != excl_dirs.end() )) {
         return false;
      }

      // TODO check file_name mask
      return true;
   }

   const bayan::options& options_;
};

} // namespace bayan