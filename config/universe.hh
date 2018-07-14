#pragma once

#include "config/light.hh"

#include <vector>

namespace config
{

///
///
///
struct universe
{
    std::vector<light> lights;
};

//
// ############################################################################
//

inline std::ostream& operator<<(std::ostream& os, const universe& u)
{
    os << "Universe:";
    for (const light& l : u.lights)
    {
        os << "\n-" << l;
    }
    return os;
}
}

