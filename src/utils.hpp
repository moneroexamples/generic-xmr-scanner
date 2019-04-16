#ifndef UTILS_HPP
#define UTILS_HPP

#include "src/MicroCore.h"

#include <any>
#include <map>
#include <string>

namespace xmreg
{

using namespace cryptonote;
using namespace crypto;

using namespace std;

using epee::string_tools::hex_to_pod;
using epee::string_tools::pod_to_hex;


map<string, std::any>
process_program_options(int argc, const char *argv[]);

}

#endif // UTILS_HPP
