
#define BOOST_TEST_MODULE bayan_test
#include <boost/test/unit_test.hpp>


#include <iostream>
#include "scanner.hpp"

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_name_filter)
{
   namespace fs = boost::filesystem;
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/file_name_mask_test"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.files_masks_.push_back(R"(test_file\S*)");

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_all_files();
   BOOST_CHECK_EQUAL(files.size(), 2);
}

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_size_filter)
{
   namespace fs = boost::filesystem;
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/file_size_test"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.min_file_sz_ = 5;

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_all_files();
   BOOST_CHECK_EQUAL(files.size(), 2);
}