#pragma once

#include "config/channel.hh"

#include <vector>

namespace config
{

///
///
///
struct light
{
    std::string name;

    uint8_t starting_address;

    std::vector<channel> channels;
};

//
// ############################################################################
//

inline std::ostream& operator<<(std::ostream& os, const light& l)
{
    os << "Light: \"" << l.name << "\" at channel: " << static_cast<uint8_t>(l.starting_address);
    for (const channel& c : l.channels)
    {
        os << "\n   " << c;
    }
    return os;
}

}
