#pragma once

#include <memory>

#include <boost/filesystem.hpp>

#define TRACE(_x_) std::cout << (_x_) << std::endl;

// ------------------------------------------------------------------
class filter {

public:
   using filter_ptr         = std::unique_ptr<filter>;
   using directory_iterator = boost::filesystem::recursive_directory_iterator;

   filter() = default;
   virtual ~filter() = default;

   void set_next_filter(filter_ptr&& next) {
      next_ = std::move(next);
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
         return false;
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

