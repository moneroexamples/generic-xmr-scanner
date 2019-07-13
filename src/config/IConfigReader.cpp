
#include "IConfigReader.h"

#if defined(__cpp_lib_filesystem)
#include <filesystem>
#else 
#include <experimental/filesystem>
namespace std 
{
    namespace filesystem = experimental::filesystem;
}
#endif

#include <fstream>
#include <iostream>

namespace xmreg
{

IConfigReader::IConfigReader(string _filepath)
    : filepath {_filepath}
{

}

bool 
IConfigReader::read()
{
    if (!filesystem::exists(filepath))
    {
        cerr << "File " << filepath 
             << " does not exist!\n";
        return false;
    }

    try
    {
        std::ifstream ifs(filepath);
        ifs >> config_data;
        return true;
    }
    catch(nl::json::parse_error const& e)
    {
        cerr << e.what() << endl;
    }

    return false;
}

nl::json 
IConfigReader::get(string _filedname) 
{
    if (!config_data.contains(_filedname))
    {
        cerr << _filedname << " not found in json\n";
        return {};
    }

    return config_data[_filedname];

}
}
