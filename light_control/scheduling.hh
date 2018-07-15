#pragma once

#include "light_control/dmx.hh"

#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <vector>

namespace light_control
{

enum class transition_type_t : uint8_t
{
    LINEAR_FADE,
    EXPONENTIAL_FADE,
    DIRECT
};

//
// ############################################################################
//

class scheduler;
struct schedule_entry
{
    ///
    /// The channels we want to transition to
    ///
    std::vector<dmx::channel_t> transition_to;

    ///
    /// How should we get to those final channels
    ///
    transition_type_t transition_type = transition_type_t::DIRECT;

    ///
    /// How long should we take to reach the final channels
    ///
    double transition_duration_s = 0.0;

    ///
    /// Function that is called after the transition is complete. This function gets
    /// access back to the scheduler, in case it want to add more things
    ///
    std::function<void(scheduler&)> wrap_up_function = [](scheduler&){};
};

//
// ############################################################################
//

class scheduler
{
public:
    scheduler() = default;

    ~scheduler() = default;

public:
    using system_clock = std::chrono::system_clock;

public:
    ///
    /// Update the given channels, if this is the last update, it may call the wrap_up_function
    ///
    void time_update(std::vector<dmx::channel_t>& channels, const system_clock::time_point& now);

    ///
    /// Set a new target, this has the option to preempt the current transition
    ///
    void queue_entry(schedule_entry entry, bool preempt=false);

    ///
    /// The number of entries that are queued up
    ///
    size_t get_num_queued_entries();

private:
    ///
    ///
    ///
    double compute_percent_complete(const system_clock::time_point& now) const;

    ///
    /// Set the next queued entry as the next executing group, set everything that needs to be set
    /// for everything to work. Return if it did something
    ///
    bool promote_next_queued_entry();

private:
    // The next thing we're transitioning to right now
    schedule_entry executing;

    // Whether the excuting entry has been set or not
    std::atomic_bool needs_executing {false};

    // This is when the transition will be complete, populated from the transition_time
    system_clock::time_point next_time;

    // List kept 1:1 with the next entries transition_to channels
    std::vector<dmx::channel_t> starting_channels;

    // These two are used for queueing up new entries
    std::deque<schedule_entry> queued_up_entries;
    std::mutex queued_up_entries_mutex;
};

}
