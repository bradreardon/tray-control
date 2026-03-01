//
// Created by andrew on 11/16/22.
//

#include "StatusNotifierWatcher.h"
#include <sdbus-c++/sdbus-c++.h>

#include "Utils.h"
#include "DBusUtils.h"
#include "Types.h"


StatusNotifierWatcher::StatusNotifierWatcher() = default;

StatusNotifierWatcher::~StatusNotifierWatcher() = default;

std::expected<void, Error> StatusNotifierWatcher::connect()
{
    return safelyExec( [this] -> std::expected<void, Error> {
        if ( proxy_ = sdbus::createProxy( sdbus::createSessionBusConnection(), sdbus::ServiceName{ "org.kde.StatusNotifierWatcher" }, sdbus::ObjectPath{ "/StatusNotifierWatcher" } ) )
            return {};
        else
            return makeError( ErrorKind::ConnectionError );
    } );
}

std::expected<std::vector<SniAddress>, Error> StatusNotifierWatcher::getRegisteredStatusNotifierItemAddresses()
{
    return
        safelyGetProperty<std::vector<std::string>>( proxy_, "org.kde.StatusNotifierWatcher", "RegisteredStatusNotifierItems" ) >>
            [] ( std::vector<std::string>&& addrs )
                -> std::expected<std::vector<SniAddress>, Error>
            {
                std::vector<SniAddress> out;
                out.reserve(addrs.size());

                for ( auto& addr : addrs )
                {
                    /*
                        StatusNotifierItems created by Ayatana are not stored at the normal path of /StatusNotifierItem,
                        so we'll split the path based on the first occurrence of `/` to separate the service name of the
                        process exposing a status item and its object path.
                    */
                    auto slashIndex = addr.find( '/' );
                    std::string serviceName = addr.substr( 0, slashIndex );

                    // leading `/` in object path must be retained
                    std::string objectPath = addr.substr( slashIndex );

                    out.push_back(SniAddress{
                        std::move( serviceName ),
                        std::move( objectPath ),
                    });
                }

                return out;
            };
}



