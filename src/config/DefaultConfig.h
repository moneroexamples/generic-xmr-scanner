
#pragma once

#include "IConfigReader.h"

namespace xmreg
{

using namespace std;
namespace nl = nlohmann; 

struct ScanningFrom
{
    string name;
    uint64_t blocks;

    static bool 
    allowed_no_of_blocks(
        vector<ScanningFrom> const& sfs,
        uint64_t no_of_blocks)
    {
      auto it = std::find_if(
              sfs.begin(), sfs.end(),
              [&no_of_blocks](auto const& sf)
              {
                return sf.blocks  == no_of_blocks;
              });

      return it != sfs.end();
    }
};

inline void 
from_json(nl::json const& j, ScanningFrom& sf)
{
    sf.name = j[0].get<string>();
    sf.blocks = j[1].get<uint64_t>();
}

inline void 
from_json(nl::json const& j, vector<ScanningFrom>& sfs)
{
    for (auto const& jsf: j)
    {
        sfs.push_back(jsf.get<ScanningFrom>());
    }
}

class DefaultConfig: public IConfigReader
{
public:
    uint64_t blocks_lookahead {10};

    DefaultConfig(string _filepath); 
    vector<ScanningFrom> scannig_from();
};

}
