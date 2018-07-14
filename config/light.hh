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
    std::vector<channel> channels;
};

//
// ############################################################################
//

inline std::ostream& operator<<(std::ostream& os, const light& l)
{
    os << "Light:";
    for (const channel& c : l.channels)
    {
        os << "\n  " << c;
    }
    return os;
}

}
