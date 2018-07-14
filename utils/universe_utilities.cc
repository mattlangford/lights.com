#include <algorithm>

#include "utils/universe_utilities.hh"

namespace utils
{

size_t get_light_end_channel(const config::light& light)
{
    size_t max_channel_base_offset = 0;
    for (const config::channel& channel : light.channels)
    {
        if (channel.base_offset > max_channel_base_offset)
        {
            max_channel_base_offset = channel.base_offset;
        }
    }

    return light.starting_address + max_channel_base_offset;
}

size_t get_num_channels(const config::universe& universe)
{
    if (universe.lights.empty())
    {
        return 0;
    }

    size_t max_channel = 0;
    for (const config::light& light : universe.lights)
    {
        if (light.starting_address >= max_channel)
        {
            max_channel = get_light_end_channel(light);
        }
    }

    return max_channel;
}

}
