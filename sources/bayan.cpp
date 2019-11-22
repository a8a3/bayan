

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include "options.hpp"

namespace po = boost::program_options;
bayan::options o;

// ------------------------------------------------------------------
int main(int argc, char** argv) {
   try {
      po::options_description opt_desc("command line options");
      opt_desc.add_options()
            ("help,h", "produce help message")
            ("include-dirs,I", po::value<std::vector<std::string>>(&o.in_dirs_),     "include directories for scan")
            ("exclude-dirs,E", po::value<std::vector<std::string>>(&o.excl_dirs_),   "exclude directories for scan")
            ("recursive,R",    po::value<int>(&o.recursive_),                        "0- scan current directory, 1- recursive scan")
            ("min-size,B",     po::value<int>(&o.min_file_sz_),                      "min file size(bytes) to scan")
            ("file-mask,M",    po::value<std::vector<std::string>>(&o.files_masks_), "files names masks to scan")
            ("block-size,S",   po::value<int>(&o.block_sz_),                         "scan block size(bytes)")
            ("conv,C",         po::value<std::string>(&o.conv_algo_),                "convolution algorithm")
      ;

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, opt_desc), vm);
      po::notify(vm);

      if (vm.count("help")) {
         std::cout << opt_desc << '\n';
         return 1;
      }

      std::cout << o;
   }
   catch (const po::error& e) {
      std::cerr << e.what() << std::endl;
   }
      
   return 0;
}