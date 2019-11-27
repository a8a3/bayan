
#define BOOST_TEST_MODULE bayan_test
#include <boost/test/unit_test.hpp>


#include <iostream>
#include "scanner.hpp"

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_name_filter_test)
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
BOOST_AUTO_TEST_CASE(file_size_filter_test)
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

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(exclude_dirs_test)
{
   namespace fs = boost::filesystem;
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/excl_dirs_test"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.excl_dirs_.push_back(std::string(test_files_dir + "/excl_dir"));
   options.recursive_ = 1;

   std::cout << options;

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_all_files();
   BOOST_CHECK_EQUAL(files.size(), 4);
}

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_chunks_reading_test)
{
   namespace fs = boost::filesystem;
   bayan::options options;
   options.block_sz_ = 1;
   
   std::cout << options;

   bayan::files_scanner scanner{options};
   const auto file_name = fs::current_path().parent_path().string() + "/tests/data/files4reading/file1.dat";

   BOOST_CHECK(fs::is_regular_file(file_name));

   bool no_eof = true;
   int chunk_ind{0};
   do {
      const auto data = bayan::files_scanner::get_file_chunk(file_name, chunk_ind, 2);
      std::cout << data.first << '\n';
      ++chunk_ind;
      no_eof = data.second;
   } while(no_eof);
}