
#pragma once

#include "IConfigReader.h"

namespace xmreg
{

using namespace std;
namespace nl = nlohmann; 

class DefaultAddresses: public IConfigReader
{
public:
    using IConfigReader::IConfigReader;

    nl::json 
    get(string nettype) override;
    
    nl::json 
    get(size_t nettype);
    
};

}
