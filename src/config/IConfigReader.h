
#pragma once

#include "nlohmann/json.hpp"

#include <string>
#include <optional>


namespace xmreg
{

using namespace std;
namespace nl = nlohmann; 

class IConfigReader
{
public:
    IConfigReader(string _filepath);
    
    virtual nl::json 
    get(string _filedname);

protected:
    bool read();

    string filepath;
    nl::json config_data;
};

}
