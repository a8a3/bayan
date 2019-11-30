
#define BOOST_TEST_MODULE bayan_test
#include <boost/test/unit_test.hpp>

#include <queue>

#include <iostream>
#include <scanner.hpp>
#include <hash.hpp>

namespace fs = boost::filesystem;

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_name_filter_test)
{
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/file_name_mask_test"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.files_masks_.push_back(R"(test_file\S*)");

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_files_to_scan();
   BOOST_CHECK_EQUAL(files.size(), 2);
}

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_size_filter_test)
{
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/file_size_test"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.min_file_sz_ = 5;

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_files_to_scan();
   BOOST_CHECK_EQUAL(files.size(), 2);
}

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(exclude_dirs_test)
{
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/excl_dirs_test"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.excl_dirs_.push_back(std::string(test_files_dir + "/excl_dir"));
   options.recursive_ = 1;

   std::cout << options;

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_files_to_scan();
   BOOST_CHECK_EQUAL(files.size(), 4);
}

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_chunks_comparison_test)
{
   bayan::options options;
   options.block_sz_ = 1;
   bayan::files_scanner scanner{options};
   const auto file1_name = fs::current_path().parent_path().string() + "/tests/data/files4reading/file1.dat";
   const auto file2_name = fs::current_path().parent_path().string() + "/tests/data/files4reading/file2.dat";

   BOOST_REQUIRE(fs::exists(file1_name));
   BOOST_REQUIRE(fs::exists(file2_name));

   bool no_eof = true;
   int chunk_ind{0};
   constexpr auto size = 2;
   const auto crc32_algo = get_hash_algorithm("crc32");
   const auto std_algo = get_hash_algorithm("std");

   do {
      const auto data1 = bayan::files_scanner::get_file_chunk(file1_name, chunk_ind*size, size);
      const auto data2 = bayan::files_scanner::get_file_chunk(file2_name, chunk_ind*size, size);

      const auto hash_crc32_1 = get_hash(data1.first, crc32_algo);
      const auto hash_std_1 = get_hash(data1.first, std_algo);
      const auto hash_crc32_2 = get_hash(data2.first, crc32_algo);
      const auto hash_std_2 = get_hash(data2.first, std_algo);

      BOOST_CHECK_EQUAL(hash_crc32_1, hash_crc32_2);
      BOOST_CHECK_EQUAL(hash_std_1, hash_std_2);

       std::cout << data1.first.size() << " : " << data1.first << ", crc32 hash 1: " << hash_crc32_1 << ", std hash: " << hash_std_1 << '\n';
       std::cout << data2.first.size() << " : " << data2.first << ", crc32 hash 2: " << hash_crc32_2 << ", std hash: " << hash_std_2 << '\n';
      ++chunk_ind;
      no_eof = data1.second || data2.second;
   } while( no_eof );
}

// ------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(file_baskets_test)
{
   const auto test_files_dir{fs::current_path().parent_path().string() + "/tests/data/files4comparison"};

   BOOST_CHECK(fs::is_directory(test_files_dir));
   bayan::options options;
   options.in_dirs_.push_back(test_files_dir);
   options.recursive_ = 0;
   options.block_sz_ = 4;
   options.conv_algo_ = "crc32";

   std::cout << options;

   bayan::files_scanner scanner{options};
   const auto files = scanner.get_files_to_scan();
   BOOST_CHECK_EQUAL(files.size(), 5);
   scanner.scan();
}