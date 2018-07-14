#pragma once

#include <string>

#include "config/light.hh"

namespace config
{

///
/// Generate a litake basic light, the starting address will be incremented to point to the next address after this light.
/// TODO: This could be done as a ymal config, maybe that makes sense
///
light generate_litake_basic_light(std::string light_name, uint16_t& address);

}
