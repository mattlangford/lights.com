#pragma once

#include "config/channel_roles.hh"
#include <cstddef>
#include <iostream>
#include <string>

namespace config
{

///
/// Single DMX channel with some meta information. Multiple channels can share the same
/// base_offset, but they will probably have different (non-overlapping) min/max values
///
struct channel
{
    ///
    /// Common name for this channel
    ///
    std::string name;

    ///
    /// Offset from the starting channel for the light this channel belongs to
    ///
    size_t base_offset;

    ///
    /// The min and max values this channel (or subchannel) can be set to
    ///
    uint8_t min_value;
    uint8_t max_value;

    ///
    /// Role for this channel, used to map it to something meaningful
    ///
    channel_role role;
};

//
// ############################################################################
//

inline std::ostream& operator<<(std::ostream& os, channel c)
{
    os << "Channel: " << c.name << " (" << c.base_offset << ") - values=["
        << static_cast<uint16_t>(c.min_value) << ", "
        << static_cast<uint16_t>(c.max_value)
        << "] role: " << c.role;
    return os;
}

}
