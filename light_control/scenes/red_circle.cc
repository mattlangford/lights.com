#include "config/channel_roles.hh"
#include "light_control/scenes/red_circle.hh"
#include "light_control/scheduling.hh"
#include "utils/scene_utilities.hh"
#include "utils/universe_utilities.hh"

#include <iostream>
#include <random>
#include <vector>

namespace
{
static constexpr float SPEED_HZ = 5.0;

struct CycleHelper
{
    CycleHelper(std::vector<dmx::channel_t> red_channels_)
        : red_channels(std::move(red_channels_))
    {
        if (red_channels.empty())
        {
            throw std::runtime_error("No red channels found!!");
        }
    }

    void operator()(light_control::scheduler& host)
    {
        std::deque<light_control::schedule_entry> entries;

        light_control::schedule_entry reference;
        reference.transition_type = light_control::transition_type_t::EXPONENTIAL_FADE;
        reference.transition_duration_s = SPEED_HZ / red_channels.size();
        for (size_t i = 0; i < red_channels.size(); ++i)
        {
            dmx::channel_t& last_channel = red_channels[i];
            dmx::channel_t& channel = red_channels[(i + 1) % red_channels.size()];

            last_channel.level = 0; // reset back to black
            channel.level = 255;

            reference.transition_to = {last_channel, channel};
            entries.emplace_back(reference);
        }

        // At the end of this transition, go ahead and generate another cycle
        entries.back().wrap_up_function = CycleHelper{std::move(red_channels)};

        // Queue these entires up with the host. We only want to do this if the queue is empty
        // (meaning that someone else hasn't come and put something in)
        host.enqueue_entries_if_empty(entries);
    }

    std::vector<dmx::channel_t> red_channels;
};

}

namespace light_control
{

std::deque<schedule_entry> red_circle::get_schedule(const config::universe& universe)
{
    const auto channel_is_red
        = [](const config::channel& c){ return c.role == config::channel_role::RED; };
    std::vector<dmx::channel_t> red_channels = utils::get_all_channels(universe, channel_is_red);

    // Fade everything off
    schedule_entry clear = utils::clear_universe(universe);
    clear.transition_type = transition_type_t::EXPONENTIAL_FADE;
    clear.transition_duration_s = 0.5;

    clear.wrap_up_function = CycleHelper{std::move(red_channels)};
    return {clear};
}

REGISTER_SCENE(red_circle, red_circle);
}
