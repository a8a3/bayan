

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include "options.hpp"
#include "scanner.hpp"

namespace po = boost::program_options;
bayan::options o;

// ------------------------------------------------------------------
int main(int argc, char** argv) {
   try {
      po::options_description opt_desc("command line options");
      opt_desc.add_options()
      ("help,h", "produce help message")
      ("include-dirs,I", po::value<std::vector<std::string>>(&o.in_dirs_),    "include directories for scan")
      ("exclude-dirs,E", po::value<std::vector<std::string>>(&o.excl_dirs_),    "exclude directories for scan")
      ("file-mask,M",    po::value<std::vector<std::string>>(&o.files_masks_),    "files names masks(as regexp) to scan")
      ("recursive,R",    po::value<unsigned int>(&o.recursive_)->default_value(0),    "0- scan current directory, 1- recursive scan")
      ("min-size,B",     po::value<unsigned int>(&o.min_file_sz_)->default_value(1),    "min file size(bytes) to scan")
      ("block-size,S",   po::value<unsigned int>(&o.block_sz_)->default_value(1),    "scan block size(bytes)")
      ("conv,C",         po::value<std::string>(&o.conv_algo_)->default_value("md5"),    "convolution algorithm")
      ;

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, opt_desc), vm);
      po::notify(vm);

      if (vm.count("help")) {
         std::cout << opt_desc << '\n';
         return 1;
      }

      std::cout << o;
      bayan::files_scanner fs(o);
      fs.scan();
   }
   catch (const po::error& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
   catch (...) {
      return 1;
   }
      
   return 0;
}