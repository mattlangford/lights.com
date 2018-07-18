#include "config/channel_roles.hh"
#include "light_control/scenes/submarine.hh"
#include "light_control/scheduling.hh"
#include "utils/scene_utilities.hh"
#include "utils/universe_utilities.hh"


#include <iostream>
#include <vector>

namespace
{
constexpr uint8_t STARTING_RED = 255;
constexpr uint8_t ENDING_RED = 50;

struct CycleHelper
{
    CycleHelper(std::vector<dmx::channel_t> red_channels)
        : red_channels(std::move(red_channels))
    {
    }

    void operator()(light_control::scheduler& host)
    {
        // First fade the red lights to the starting color
        light_control::schedule_entry start;
        for (dmx::channel_t& channel : red_channels)
        {
            channel.level = STARTING_RED;
        }
        start.transition_to = red_channels;
        start.transition_type = light_control::transition_type_t::LINEAR_FADE;
        start.transition_duration_s = 0.1;

        // Then fade the red lights to the ending color
        light_control::schedule_entry end;
        for (dmx::channel_t& channel : red_channels)
        {
            channel.level = ENDING_RED;
        }
        end.transition_to = red_channels;
        end.transition_type = light_control::transition_type_t::EXPONENTIAL_FADE;
        end.transition_duration_s = 2.0;

        // At the end of this transition, go ahead and generate another cycle
        end.wrap_up_function = CycleHelper{std::move(red_channels)};

        // Queue these entires up with the host. We only want to do this if the queue is empty
        // (meaning that someone else hasn't come and put something in)
        host.enqueue_entries_if_empty({std::move(start), std::move(end)});
    }

    std::vector<dmx::channel_t> red_channels;
};

}

namespace light_control
{

std::deque<schedule_entry> submarine::get_schedule(const config::universe& universe)
{
    const auto channel_is_red
        = [](const config::channel& c){ return c.role == config::channel_role::RED; };
    std::vector<dmx::channel_t> red_channels = utils::get_all_channels(universe, channel_is_red);

    // set all the red channels to be on
    schedule_entry clear = utils::clear_universe(universe);
    for (dmx::channel_t& channel : clear.transition_to)
    {
        for (dmx::channel_t& red_channel : red_channels)
        {
            if (red_channel.address == channel.address)
            {
                channel.level = ENDING_RED;
            }
        }
    }

    clear.transition_type = transition_type_t::EXPONENTIAL_FADE;
    clear.transition_duration_s = 0.5;
    clear.wrap_up_function = CycleHelper{std::move(red_channels)};
    return {clear};
}

REGISTER_SCENE(submarine, submarine);
}
