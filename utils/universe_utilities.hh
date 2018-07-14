#pragma once

#include <cstddef>

#include "config/channel.hh"
#include "config/light.hh"
#include "config/universe.hh"

namespace utils
{

size_t get_light_end_channel(const config::light& light);

size_t get_num_channels(const config::universe& universe);

}
