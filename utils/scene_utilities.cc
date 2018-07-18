#include "config/channel_roles.hh"
#include "light_control/dmx.hh"
#include "utils/scene_utilities.hh"
#include "utils/universe_utilities.hh"

#include <vector>

namespace utils
{

light_control::schedule_entry clear_universe(const config::universe& universe)
{
    // these should be set to 100%
    const auto channel_is_brightness
        = [](const config::channel& c){ return c.role == config::channel_role::BRIGHTNESS; };
    const std::vector<dmx::channel_t> brightness_channels
        = get_all_channels(universe, channel_is_brightness);

    light_control::schedule_entry set_black;
    set_black.transition_duration_s = 0.5;
    set_black.transition_type = light_control::transition_type_t::EXPONENTIAL_FADE;

    // get all the channels
    set_black.transition_to = get_all_channels(universe);
    for (dmx::channel_t& channel : set_black.transition_to)
    {
        channel.level = 0;
        for (const dmx::channel_t& brightness : brightness_channels)
        {
            if (brightness.address == channel.address)
            {
                // TODO: Move channels over to storing percents
                channel.level = 127;
            }
        }
    }

    return set_black;
}

}
