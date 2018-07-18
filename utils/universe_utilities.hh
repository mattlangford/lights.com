#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include "config/channel_roles.hh"
#include "config/channel.hh"
#include "config/light.hh"
#include "config/universe.hh"

#include "light_control/dmx.hh"

namespace utils
{

///
/// Get the last channel this light owns
///
size_t get_light_end_channel(const config::light& light);

///
/// Get the number of channels in this universe
///
size_t get_num_channels(const config::universe& universe);

///
/// Get all the channels in the universe (with the correct addresses)
///
std::vector<dmx::channel_t> get_all_channels(const config::universe& universe,
                                             const std::function<bool(const config::channel&)>& predicate);

inline std::vector<dmx::channel_t> get_all_channels(const config::universe& universe)
{
    const auto always_return_true = [](const config::channel&){ return true; };
    return get_all_channels(universe, always_return_true);
}

}
