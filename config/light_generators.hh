#pragma once

#include <string>

#include "config/light.hh"

namespace config
{

/// TODO: These could be done as a ymal config, maybe that makes sense

///
/// Generate a litake basic light, the starting address will be incremented to point to the next address after this light.
///
light generate_litake_basic_light(std::string light_name, uint16_t& address);

///
/// Generate an lm70 moving light, the starting address will be incremented to point to the next address after this
/// light.
///
light generate_lm70_moving_light(std::string light_name, uint16_t& address);

}
