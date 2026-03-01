#pragma once

#include <ostream>

struct SniAddress {
    std::string serviceName;
    std::string objectPath;
};


// Overload the concatenation operator so that we can easily reference SNI addresses in log/error messages
inline std::ostream& operator<<(std::ostream& os, const SniAddress& addr)
{
    return os << addr.serviceName << addr.objectPath;
}