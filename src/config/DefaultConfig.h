
#pragma once

#include "IConfigReader.h"

namespace xmreg
{

using namespace std;
namespace nl = nlohmann; 

class DefaultConfig: public IConfigReader
{
public:

    DefaultConfig(string _filepath); 
    nl::json scannig_from();

};

}
