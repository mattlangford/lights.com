#include "light_control/light_universe_controller.hh"
#include "utils/universe_utilities.hh"

#include "logging.hh"

namespace light_control
{

//
// ############################################################################
//

light_universe_controller::light_universe_controller(serial::abstract_serial_interface& connection,
                                                     const controller_params& params,
                                                     const config::universe& universe)
    : connection_(connection),
      universe_(universe),
      channels_(utils::get_num_channels(universe) + 1),
      last_update_time_(std::chrono::high_resolution_clock::now()),
      running_(false)
{
    for (size_t i = 0; i < channels_.size(); ++i)
    {
        dmx::channel_t& channel = channels_[i];
        channel.address = i;
        channel.level = 0;
    }

    // should happen after everything has been set up
    update_params(params);
}

//
// ############################################################################
//

light_universe_controller::~light_universe_controller()
{
    running_ = false;

    if (executive_handle_.joinable())
    {
        executive_handle_.join();
    }
}

//
// ############################################################################
//

void light_universe_controller::do_update()
{
    // get the latest channel values
    scheduler_.time_update(channels_, scheduler::system_clock::now());

    // message, where each bool is a bit in the data stream
    const std::vector<bool> message = dmx::dmx_helper::generate_message_from_channels(channels_);

    // packed message, where each byte is contains a single bit of data, which will be used to simulate a
    // differential pair
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

    // This is kind of a hack. If the serial update didn't go through, then we want to wait for a second before
    // sending the next update
    while (connection_.write_data(packed_message_to_send) == false)
    {
        LOG_WARN("Unable to send serial data, trying to resend in 1 second");
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
    }
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
