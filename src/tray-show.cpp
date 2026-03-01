//
// Created by andrew on 11/17/22.
//
#include <cxxopts.hpp>
#include <iostream>
#include <fmt/printf.h>
#include <nlohmann/json.hpp>

#include "StatusNotifierWatcher.h"
#include "StatusNotifierItem.h"
#include "StatusNotifierItemJson.h"
#include "Utils.h"
#include "Types.h"

using json = nlohmann::json;


void exitWithMsg( std::string_view msg, int code = -1 )
{
    std::cerr << msg << std::endl;
    exit( code );
}

void print_json_string (const json& obj, const char* key, const char* label_fmt) {
    auto it = obj.find(key);
    if (it == obj.end() || it->is_null()) return;

    // if the value isn't a string, skip (or coerce)
    if (!it->is_string()) return;

    fmt::printf(label_fmt, it->get_ref<const std::string&>().c_str());
};

void print_json_u32 (const json& obj, const char* key, const char* label_fmt) {
    auto it = obj.find(key);
    if (it == obj.end() || it->is_null()) return;

    if (!it->is_number_unsigned()) return;

    fmt::printf(label_fmt, it->get<uint32_t>());
};


int main( int argc, char** argv )
{
    cxxopts::Options optionsDecl( "tray-show", "Show items in the system tray" );
    optionsDecl.add_options()
        ( "h,help", "Print help and exit", cxxopts::value<bool>()->default_value("false") )
        ( "j,json", "Print all item properties in JSON format", cxxopts::value<bool>()->default_value("false") )
        ( "v,verbose", "Show full info about each item", cxxopts::value<bool>()->default_value("false") );

    const auto options = optionsDecl.parse( argc, argv );
    if ( options["help"].as<bool>() )
    {
        std::cout << optionsDecl.help();
        return 0;
    }
    const bool verboseOutput = options["verbose"].as<bool>();
    const bool jsonOutput = options["json"].as<bool>();

    StatusNotifierWatcher watcher;
    if ( auto connRes = watcher.connect(); !connRes )
        exitWithMsg( "Could not connect to the StatusNotifierWatcher with error: " + connRes.error().show(), -1 );

    if ( auto maybeAddrs = watcher.getRegisteredStatusNotifierItemAddresses() )
    {
        // collect all item data as JSON first, store in a vector
        std::vector<json> itemData;
        itemData.reserve( maybeAddrs.value().size() );

        for ( const auto& addr : maybeAddrs.value() )
        {
            StatusNotifierItem item( addr );
            if ( auto connRes = item.connect(); auto dbusConnRes = item.connectDBus() )
            {
                // Serialize, print item data with 4-space indent
                json j;
                to_json( j, item );

                // Add the D-Bus address we used to fetch the information
                json jAddr;
                to_json( jAddr, addr );
                j["address"] = jAddr;

                // Push item data onto the vector
                itemData.push_back( j );
            }
            else
            {
                std::cerr << "Could not connect to the StatusNotifierItem on address: " << addr << " with error: " << connRes.error().show() << '\n';
            }
        }

        // Print collected data in chosen output format
        if ( jsonOutput ) {
            // Serialize, print item data with 4-space indent
            json j = itemData;
            std::cout << j.dump( 4 ) << '\n';
        }
        else
        {
            for ( const json& item : itemData )
            {
                print_json_string( item, "category",     "Category: %s\n" );
                print_json_string( item, "title",        "Title: %s\n" );

                if (verboseOutput)
                {
                    print_json_string( item, "id",       "Id: %s\n" );
                    print_json_string( item, "status",   "Status: %s\n" );
                    print_json_u32( item, "windowId",    "WindowId: %u\n" );
                    print_json_string( item, "iconName", "IconName: %s\n" );
                }
                std::cout << '\n';
            }
        }
    }
}