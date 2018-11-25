#pragma once

#include <cstddef>
#include <iostream>

namespace config
{

///
/// Possible channel roles that a channel can play. This helps map generic light
/// channels to more meaningful and useful channels
///
enum class channel_role : uint8_t
{
    RED,
    GREEN,
    BLUE,
    WHITE,
    BRIGHTNESS,
    FLASH,
    PAN,
    TILT,
    NONE
};

//
// ############################################################################
//

inline std::ostream& operator<<(std::ostream& os, channel_role role)
{
    switch(role)
    {
    case channel_role::RED: { os << "RED"; break; }
    case channel_role::GREEN: { os << "GREEN"; break; }
    case channel_role::BLUE: { os << "BLUE"; break; }
    case channel_role::WHITE: { os << "WHITE"; break; }
    case channel_role::BRIGHTNESS: { os << "BRIGHTNESS"; break; }
    case channel_role::FLASH: { os << "FLASH"; break; }
    case channel_role::PAN: { os << "PAN"; break; }
    case channel_role::TILT: { os << "TILT"; break; }
    case channel_role::NONE: { os << "NONE"; break; }
    }

    return os;
}
}
