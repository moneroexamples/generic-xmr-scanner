#ifndef UTILS_HPP
#define UTILS_HPP

#include "src/MicroCore.h"

#include <any>
#include <map>
#include <string>

#if defined(__cpp_lib_filesystem)
#include <filesystem>
#else 
#include <experimental/filesystem>
namespace std 
{
    namespace filesystem = experimental::filesystem;
}
#endif
namespace xmreg
{

using namespace cryptonote;
using namespace crypto;

using namespace std;

using epee::string_tools::hex_to_pod;
using epee::string_tools::pod_to_hex;


map<string, std::any>
process_program_options(int argc, const char *argv[]);

inline auto
get_current_timestamp() 
{
   return  chrono::duration_cast<chrono::seconds>(
          chrono::system_clock::now().time_since_epoch());
}

filesystem::path 
expand(filesystem::path in);

}

#endif // UTILS_HPP
