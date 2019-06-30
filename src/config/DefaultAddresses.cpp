#include "DefaultAddresses.h"

#include <iostream>

namespace xmreg
{

using namespace std;
namespace nl = nlohmann; 

nl::json 
DefaultAddresses::get(string nettype)
{
    if (!read())
    {
        return {};
    }

    if (!config_data.contains(nettype))
    {
        return {};
    }

    return config_data[nettype];
}
    
nl::json 
DefaultAddresses::get(size_t nettype)
{
    switch(nettype)
    {
        case 0: {return get("mainnet");}
        case 1: {return get("testnet");}
        case 2: {return get("stagenet");}
        default: {return {};}
    }
}

}
