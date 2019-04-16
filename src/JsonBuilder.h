#pragma once


#include "nlohmann/json.hpp"

#include <string>

namespace xmreg
{

namespace nl = nlohmann;


struct JsonBuilder
{
    nl::json j;

    JsonBuilder& success(nl::json jdata)
    {
        j["status"] = "success";
        j["data"] = std::move(jdata);
        return *this; 
    }
    
    JsonBuilder& fail(nl::json jdata)
    {
        j["status"] = "fail";
        j["data"] = std::move(jdata);
        return *this; 
    }
    
    JsonBuilder& error(std::string msg)
    {
        j["status"] = "error";
        j["message"] = std::move(msg);
        return *this; 
    }

    std::string dump() const
    {
        return j.dump();
    }
};


inline void 
to_json(nl::json& j, JsonBuilder const& jb)
{
    j = jb.j;
}

using jb = JsonBuilder;

    
}
