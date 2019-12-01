#pragma once

#include <memory>
#include <regex>
#include <boost/filesystem.hpp>


// ------------------------------------------------------------------
class filter {

public:
   using filter_ptr         = std::unique_ptr<filter>;
   using directory_iterator = boost::filesystem::recursive_directory_iterator;

   filter() = default;
   virtual ~filter() = default;

   void set_next_filter(filter_ptr&& next) {
      if (next_) {
         next_->set_next_filter(std::move(next));
      } else {
         next_ = std::move(next);
      }
   }

   bool apply(directory_iterator& iter) const {
      if (do_filter(iter)) {
         if (next_) {
            return next_->apply(iter);
         }
         return true;
      }
      return false;
   }

private:
   virtual bool do_filter(directory_iterator& iter) const = 0;
   filter_ptr next_;
};

// ------------------------------------------------------------------
class file_size_filter : public filter {
public:
   file_size_filter(unsigned int size) : size_(size) {}

   bool do_filter(directory_iterator& iter) const override {
      namespace fs = boost::filesystem;

      if (fs::is_directory(*iter)) {
         return true;
      }

      const auto file_name = iter->path().string();
      boost::system::error_code ec;
      const auto file_size = fs::file_size(file_name, ec);

      if (!ec) {
         return file_size >= size_;
      }
      std::cout << "file '" << file_name << "' attribute reading error: " << ec.message() << std::endl;
      return false;
   }

private:
   const unsigned int size_;
};

// ------------------------------------------------------------------
class file_name_filter : public filter {
public:
   file_name_filter(const std::vector<std::string>& files_masks) : files_masks_(files_masks) {}

   bool do_filter(directory_iterator& iter) const override {
      namespace fs = boost::filesystem;
      const auto file_name = iter->path().filename().string();

      const auto match_by_mask = [&file_name] (const auto& condition) {
         const std::regex filter{condition};
         return std::regex_match(file_name, filter);
      };
      return std::all_of(files_masks_.cbegin(), files_masks_.cend(), match_by_mask);
   }

private:
   const std::vector<std::string>& files_masks_;
};

// ------------------------------------------------------------------
class directories_filter : public filter {
public:
   directories_filter(const std::vector<std::string>& excl_dirs, unsigned int recursive)
   : excl_dirs_(excl_dirs)
   , recursive_(recursive){}

   bool do_filter(directory_iterator& iter) const override {
      namespace fs = boost::filesystem;

      if (fs::is_directory(iter->path())) {
         if (recursive_ == 0 || std::find(excl_dirs_.cbegin(), excl_dirs_.cend(), iter->path().string()) != excl_dirs_.cend()) {
            iter.no_push();
         }
         return false;
      }
      return true;
   }

private:
   const std::vector<std::string>& excl_dirs_;
   const unsigned int recursive_;
};
