#include "config/channel_roles.hh"
#include "light_control/scenes/aurora.hh"
#include "light_control/scheduling.hh"
#include "utils/scene_utilities.hh"
#include "utils/universe_utilities.hh"

#include <iostream>
#include <random>
#include <vector>

namespace
{
static constexpr uint8_t MIN_GREEN = 50;
static constexpr uint8_t MIN_BLUE = 50;
static constexpr double MIN_FADE = 5.0;

static constexpr uint8_t MAX_GREEN = 80;
static constexpr uint8_t MAX_BLUE = 100;
static constexpr double MAX_FADE = 8.0;

static float genearte_random_number(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(min, max);

    return dist(mt);
}

struct CycleHelper
{
    CycleHelper(std::vector<dmx::channel_t> green_channels, std::vector<dmx::channel_t> blue_channels)
        : green_channels(std::move(green_channels)),
          blue_channels(std::move(blue_channels))
    {
    }

    light_control::schedule_entry generate_entry()
    {
        light_control::schedule_entry entry;
        for (dmx::channel_t& channel : green_channels)
        {
            channel.level = genearte_random_number(MIN_GREEN, MAX_GREEN);
            entry.transition_to.push_back(channel);
        }
        for (dmx::channel_t& channel : blue_channels)
        {
            channel.level = genearte_random_number(MIN_BLUE, MAX_BLUE);
            entry.transition_to.push_back(channel);
        }

        entry.transition_type = light_control::transition_type_t::EXPONENTIAL_FADE;
        entry.transition_duration_s = genearte_random_number(MIN_FADE, MAX_FADE);

        return entry;
    }

    void operator()(light_control::scheduler& host)
    {
        std::deque<light_control::schedule_entry> entries;
        constexpr size_t NUM_TO_QUEUE = 20;
        for (size_t i = 0; i < NUM_TO_QUEUE; ++i)
        {
            entries.emplace_back(generate_entry());
        }

        // At the end of this transition, go ahead and generate another cycle
        entries.back().wrap_up_function = CycleHelper{std::move(green_channels), std::move(blue_channels)};

        // Queue these entires up with the host. We only want to do this if the queue is empty
        // (meaning that someone else hasn't come and put something in)
        host.enqueue_entries_if_empty(entries);
    }

    std::vector<dmx::channel_t> green_channels;
    std::vector<dmx::channel_t> blue_channels;
};

}

namespace light_control
{

std::deque<schedule_entry> aurora::get_schedule(const config::universe& universe)
{
    const auto channel_is_green
        = [](const config::channel& c){ return c.role == config::channel_role::GREEN; };
    std::vector<dmx::channel_t> green_channels = utils::get_all_channels(universe, channel_is_green);

    const auto channel_is_blue
        = [](const config::channel& c){ return c.role == config::channel_role::BLUE; };
    std::vector<dmx::channel_t> blue_channels = utils::get_all_channels(universe, channel_is_blue);

    // set all the red channels to be on
    schedule_entry clear = utils::clear_universe(universe);
    for (dmx::channel_t& channel : clear.transition_to)
    {
        for (dmx::channel_t& green_channel : green_channels)
            if (green_channel.address == channel.address)
                channel.level = MIN_GREEN;

        for (dmx::channel_t& blue_channel : blue_channels)
            if (blue_channel.address == channel.address)
                channel.level = MIN_BLUE;
    }

    clear.transition_type = transition_type_t::EXPONENTIAL_FADE;
    clear.transition_duration_s = 0.5;

    clear.wrap_up_function = CycleHelper{std::move(green_channels), std::move(blue_channels)};
    return {clear};
}

REGISTER_SCENE(aurora, aurora);
}
