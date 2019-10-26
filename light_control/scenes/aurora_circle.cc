#include "config/channel_roles.hh"
#include "light_control/scenes/aurora_circle.hh"
#include "light_control/scheduling.hh"
#include "utils/scene_utilities.hh"
#include "utils/universe_utilities.hh"

#include <iostream>
#include <random>
#include <vector>

namespace
{

static constexpr uint8_t MAX_LEVEL = 175;

static constexpr uint8_t MIN_GREEN = 70;
static constexpr uint8_t MIN_BLUE = 75;

static constexpr uint8_t MAX_GREEN = 80;
static constexpr uint8_t MAX_BLUE = 100;

static constexpr double FADE_TIME = 0.5;

static float genearte_random_number(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(min, max);

    return dist(mt);
}

struct CycleHelper
{
    CycleHelper(std::vector<dmx::channel_t> red_channels, std::vector<dmx::channel_t> green_channels, std::vector<dmx::channel_t> blue_channels)
        : red_channels(std::move(red_channels)),
          green_channels(std::move(green_channels)),
          blue_channels(std::move(blue_channels))
    {
        if (red_channels.size() != green_channels.size() || green_channels.size() != blue_channels.size())
        {
            throw std::runtime_error("Invalid channels!");
        }
    }

    light_control::schedule_entry generate_entry(size_t white_i)
    {
        const size_t size = red_channels.size();

        light_control::schedule_entry entry;
        for (size_t i = 0; i < size; ++i)
        {
            auto& r = red_channels[i];
            auto& g = green_channels[i];
            auto& b = blue_channels[i];

            //
            // Clear whatever the last channel's red was, this is because it was set to max
            //
            if (i == white_i)
            {
                r.level = MAX_LEVEL;
                b.level = MAX_LEVEL;
                g.level = MAX_LEVEL;
            }
            else if (abs(i - white_i) == 1)
            {
                r.level = MAX_LEVEL / 2;
                b.level = MAX_LEVEL / 2;
                g.level = MAX_LEVEL / 2;
            }
            else if(i % 3 == 0)
            {
                r.level = genearte_random_number(MIN_GREEN, MAX_GREEN);
                b.level = genearte_random_number(MIN_BLUE, MAX_BLUE);
                g.level = 0;
            }
            else
            {
                r.level = 0;
                b.level = genearte_random_number(MIN_BLUE, MAX_BLUE);
                g.level = genearte_random_number(MIN_GREEN, MAX_GREEN);
            }
            entry.transition_to.push_back(r);
            entry.transition_to.push_back(g);
            entry.transition_to.push_back(b);
        }

        entry.transition_type = light_control::transition_type_t::LINEAR_FADE;
        entry.transition_duration_s = FADE_TIME;

        return entry;
    }

    void operator()(light_control::scheduler& host)
    {
        std::deque<light_control::schedule_entry> entries;
        for (size_t i = 0; i < red_channels.size(); ++i)
        {
            entries.emplace_back(generate_entry(i));
        }
        for (size_t i = 0; i < 2 * red_channels.size(); ++i)
        {
            entries.emplace_back(generate_entry(100));
        }

        // At the end of this transition, go ahead and generate another cycle
        entries.back().wrap_up_function = CycleHelper{std::move(red_channels), std::move(green_channels), std::move(blue_channels)};

        // Queue these entires up with the host. We only want to do this if the queue is empty
        // (meaning that someone else hasn't come and put something in)
        host.enqueue_entries_if_empty(entries);
    }

    std::vector<dmx::channel_t> red_channels;
    std::vector<dmx::channel_t> green_channels;
    std::vector<dmx::channel_t> blue_channels;
};

}

namespace light_control
{

std::deque<schedule_entry> aurora_circle::get_schedule(const config::universe& universe)
{
    const auto channel_is_green
        = [](const config::channel& c){ return c.role == config::channel_role::GREEN; };
    std::vector<dmx::channel_t> green_channels = utils::get_all_channels(universe, channel_is_green);

    const auto channel_is_blue
        = [](const config::channel& c){ return c.role == config::channel_role::BLUE; };
    std::vector<dmx::channel_t> blue_channels = utils::get_all_channels(universe, channel_is_blue);

    const auto channel_is_red
        = [](const config::channel& c){ return c.role == config::channel_role::RED; };
    std::vector<dmx::channel_t> red_channels = utils::get_all_channels(universe, channel_is_red);

    // set all the red channels to be on
    schedule_entry clear = utils::clear_universe(universe);
    for (dmx::channel_t& channel : clear.transition_to)
    {
        for (dmx::channel_t& red_channel : red_channels)
            if (red_channel.address == channel.address)
                channel.level = 0;

        for (dmx::channel_t& green_channel : green_channels)
            if (green_channel.address == channel.address)
                channel.level = 0;

        for (dmx::channel_t& blue_channel : blue_channels)
            if (blue_channel.address == channel.address)
                channel.level = 0;
    }

    clear.transition_type = transition_type_t::EXPONENTIAL_FADE;
    clear.transition_duration_s = 0.5;

    clear.wrap_up_function = CycleHelper{std::move(red_channels), std::move(green_channels), std::move(blue_channels)};
    return {clear};
}

REGISTER_SCENE(aurora_circle, aurora_circle);
}
