#include "light_control/light_universe_controller.hh"
#include "light_control/dmx.hh"

#include <iostream>

namespace lights
{

light_universe_controller::light_universe_controller(serial::abstract_serial_interface& connection, const controller_params& params)
    : connection_(connection),
      last_update_time_(std::chrono::high_resolution_clock::now())
{
    valid_addresses_.fill(true);

    update_params(params);
}

//
// ############################################################################
//

light_universe_controller::~light_universe_controller()
{
    running_ = false;

    for (lights::abstract_light::ptr light : lights_)
    {
        light->set_off();
    }

    if (executive_handle_.joinable())
    {
        executive_handle_.join();
    }

    // one last update to turn off lights
    do_update();
}

//
// ############################################################################
//

void light_universe_controller::add_light_to_universe(abstract_light::ptr light)
{
    for (size_t i = light->get_start_address(); i <= light->get_end_address(); ++i)
    {
        if (valid_addresses_[i] == false)
        {
            throw std::runtime_error("Multiple lights are sharing the address (" + std::to_string(i) + ").");
        }

        valid_addresses_[i] = false;
    }

    lights_.emplace_back(std::move(light));
}

//
// ############################################################################
//

void light_universe_controller::do_update()
{
    dmx::dmx_helper::channels_t channels;
    channels.reserve(dmx::MAX_NUM_CHANNELS);

    for (abstract_light::ptr& light : lights_)
    {
        for (auto&& channel : light->get_channels())
        {
            channels.emplace_back(std::move(channel));
        }
    }

    // message, where each bool is a bit in the data stream
    const std::vector<bool> message = dmx::dmx_helper::generate_message_from_channels(channels);

    // packed message, where each byte is contains a single bit of data, which will be used to simulate a differential pair
    const serial::ByteVector_t packed_message_to_send = dmx::dmx_helper::simulate_differential_pair(message);

    // the user may call do_update more than 44 times a second, if we're configured to stop that, sleep here
    if (params_.enforce_44hz)
    {
        constexpr std::chrono::duration<double> MIN_DURATION {1 / 44.0};
        const auto now = std::chrono::high_resolution_clock::now();
        if (now - last_update_time_ < MIN_DURATION)
        {
            std::this_thread::sleep_until(last_update_time_ + MIN_DURATION);
        }

        // keep track of the current time for the next time this is called
        auto updated_now = std::chrono::high_resolution_clock::now();
        last_update_time_ = std::move(updated_now);
    }

    connection_.write_data(packed_message_to_send);
}

//
// ############################################################################
//

void light_universe_controller::update_params(const controller_params& params)
{
    // kill the thread if it's already running
    if (running_ && executive_handle_.joinable())
    {
        executive_handle_.join();
    }

    // set the internal params
    params_ = params;

    // now configure the new executive (if needed)
    switch (params_.control)
    {
    case control_type::EXECUTIVE_AUTO: // fall through
        params_.executive_rate = 1 / 44.0;
    case control_type::EXECUTIVE_SET_RATE:
        update_period_ = std::chrono::duration<double>(params_.executive_rate);
        running_ = true;
        executive_handle_ = std::thread([this](){executive_thread();});
        break;
    case control_type::MANUAL:
        // nothing here
        break;
    default:
        throw std::runtime_error("Unknown control type passed into light_universe_controller");
    }

}

//
// ############################################################################
//

void light_universe_controller::executive_thread()
{
    while (running_)
    {
        std::this_thread::sleep_for(update_period_);
        do_update();
    }
}

}
