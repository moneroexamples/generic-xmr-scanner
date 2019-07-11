#include "DefaultConfig.h"

#include <iostream>

namespace xmreg
{

using namespace std;
namespace nl = nlohmann; 

DefaultConfig::DefaultConfig(string _filepath)
    : IConfigReader(std::move(_filepath))
{
    if (!read())
    {
        throw std::runtime_error(
                "Cant read: " + _filepath );
    }
}

vector<ScanningFrom>
DefaultConfig::scannig_from() 
{
    return get("scanning_from")
        .get<vector<ScanningFrom>>();
}

}
