#include "light_control/scheduling.hh"

#include <cmath>
#include <limits>
#include <iostream>

namespace
{

///
/// Directly maps between input and output
///
constexpr static double linear_map(const double x)
{
    return x;
}

///
/// Does a smooth exponential like map between input and output
///
static double exponential_map(const double x)
{
    constexpr double VERTICAL_OFFSET = 1;
    constexpr double SCALE_FACTOR = 0.5;
    return 1.0 - (std::cos(x * M_PI) + VERTICAL_OFFSET) * SCALE_FACTOR;
}

///
/// Goes direly from 0.0 to 1.0 when the transition completes
///
constexpr static double direct_map(const double x)
{
    return x >= 1.0 - std::numeric_limits<double>::epsilon() ? 1.0 : 0.0;
}
}

namespace light_control
{

//
// ############################################################################
//

void scheduler::time_update(std::vector<dmx::channel_t>& channels, const system_clock::time_point& now)
{
    if (needs_executing == false)
    {
        if (promote_next_queued_entry() == false)
        {
            return;
        }
    }

    double percent = compute_percent_complete(now);

    // Mark if starting_channels needs to be set, if so we'll populate it during this time update
    bool starting_channels_needs_to_be_set = starting_channels.empty();

    // Holds the channels we're going to change in the same order transition_to
    for (size_t i = 0; i < executing.transition_to.size(); ++i)
    {
        const dmx::channel_t& to_channel = executing.transition_to[i];

        for (dmx::channel_t& channel : channels)
        {
            if (channel.address != to_channel.address)
                continue;

            if (starting_channels_needs_to_be_set)
                starting_channels.emplace_back(channel);

            dmx::channel_t& start = starting_channels[i];
            int level_delta = (int)to_channel.level - (int)start.level;

            channel.level = start.level + percent * level_delta;
        }
    }

    // We're done here, do a bit of cleanup
    if (percent >= 1.0)
    {
        executing.wrap_up_function(*this);

        needs_executing = false;
        promote_next_queued_entry();
    }
}

//
// ############################################################################
//

void scheduler::enqueue_entry(schedule_entry entry, bool preempt)
{
    {
        std::lock_guard<std::mutex> lock(queued_up_entries_mutex);

        if (preempt)
            queued_up_entries.clear();

        queued_up_entries.emplace_back(std::move(entry));
    }

    if (preempt)
    {
        promote_next_queued_entry();
    }
}

//
// ############################################################################
//

void scheduler::enqueue_entries(std::deque<schedule_entry> entries, bool preempt)
{
    {
        std::lock_guard<std::mutex> lock(queued_up_entries_mutex);

        if (preempt)
            queued_up_entries.clear();

        queued_up_entries.insert(queued_up_entries.cend(), entries.begin(), entries.end());
    }

    if (preempt)
        promote_next_queued_entry();
}

//
// ############################################################################
//

void scheduler::enqueue_entry_if_empty(schedule_entry entry)
{
    std::lock_guard<std::mutex> lock(queued_up_entries_mutex);
    if (queued_up_entries.empty())
        queued_up_entries.emplace_back(std::move(entry));
}

//
// ############################################################################
//

void scheduler::enqueue_entries_if_empty(std::deque<schedule_entry> entries)
{
    std::lock_guard<std::mutex> lock(queued_up_entries_mutex);
    if (queued_up_entries.empty())
        queued_up_entries.insert(queued_up_entries.cend(), entries.begin(), entries.end());
}

//
// ############################################################################
//

size_t scheduler::get_num_queued_entries()
{
    std::lock_guard<std::mutex> lock(queued_up_entries_mutex);
    return queued_up_entries.size();
}

//
// ############################################################################
//

double scheduler::compute_percent_complete(const system_clock::time_point& now) const
{
    // Compute the time left as a percentage of the original transition time
    const double& transition_time = executing.transition_duration_s;
    const double transition_left
        = std::chrono::duration_cast<std::chrono::milliseconds>(next_time - now).count() / 1000.;
    const double fraction_left = transition_left / std::max(transition_time, 1E-3);

    // Ranges between 0 and 1
    const double fraction = std::min(std::max(0.0, 1.0 - fraction_left), 1.0);

    // Map the fraction along to a percent of transition completion, using the transition type
    switch (executing.transition_type)
    {
    case transition_type_t::LINEAR_FADE:
        return linear_map(fraction);
    case transition_type_t::EXPONENTIAL_FADE:
        return exponential_map(fraction);
    case transition_type_t::DIRECT:
        return direct_map(fraction);
    default:
        throw std::runtime_error("Unknown transition type!");
    }
}

//
// ############################################################################
//

bool scheduler::promote_next_queued_entry()
{
    std::lock_guard<std::mutex> lock(queued_up_entries_mutex);
    if (queued_up_entries.empty())
    {
        return false;
    }

    executing = std::move(queued_up_entries.front());
    queued_up_entries.pop_front();

    next_time =
        system_clock::now() +
        std::chrono::duration_cast<system_clock::duration>(
            std::chrono::duration<double>(executing.transition_duration_s));
    starting_channels.clear();

    needs_executing = true;

    return true;
}
}
