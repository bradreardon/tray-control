#pragma once

#include <ostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


struct SniAddress {
    std::string serviceName;
    std::string objectPath;
};


// Overload the concatenation operator so that we can easily reference SNI addresses in log/error messages
inline std::ostream& operator<<(std::ostream& os, const SniAddress& addr)
{
    return os << addr.serviceName << addr.objectPath;
}

// Implement JSON serialization for SniAddress
inline void to_json(json& j, const SniAddress& t)
{
    j = nlohmann::json{
        {"serviceName", t.serviceName},
        {"objectPath",  t.objectPath},
    };
}