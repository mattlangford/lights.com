#pragma once
#include "config/universe.hh"
#include "light_control/dmx.hh"
#include "serial_control/abstract_serial_interface.hh"

#include <atomic>
#include <memory>
#include <thread>

namespace lights
{

class light_universe_controller
{
public:
    enum class control_type : uint8_t
    {
        EXECUTIVE_AUTO, // executive thread spun up, it will preform updates as fast as DMX allows
        EXECUTIVE_SET_RATE, // executive thread spun up, it will run as fast as configured
        MANUAL // no thread spun up, you'll have to manually call do_update
    };

    struct controller_params
    {
        //
        // How should this universe be controlled?
        //
        control_type control = control_type::EXECUTIVE_AUTO;

        //
        // if control == EXECUTIVE_SET_RATE
        //
        double executive_rate = 0.0;

        //
        // Should we block and wait to enforce we're meeting the spec with the rate we send
        // DMX universe updates
        //
        bool enforce_44hz = false;
    };

public:
    light_universe_controller(serial::abstract_serial_interface& connection,
                              const controller_params& params,
                              const config::universe& universe);

    ~light_universe_controller();

public:
    // For anything that is expecting to change the channels will need them. The vector won't be resized after construction
    inline std::shared_ptr<std::vector<dmx::channel_t>> get_underlying_channels() { return channels_; }

    // preform an update
    void do_update();

    // update the internal params
    void update_params(const controller_params& params);

private:
    // runs do_update every update_period seconds
    void executive_thread();

private:
    // connection to the board that interfaces with the lights
    serial::abstract_serial_interface& connection_;

    // parameters for this controller
    controller_params params_;

    // the universe that we're controlling and the channels that we use for updates
    const config::universe universe_;
    std::shared_ptr<std::vector<dmx::channel_t>> channels_;

    // how long to sleep after each update
    std::chrono::duration<double> update_period_;

    // if we've been configured to enforce the 44hz DMX update rate, we'll need to keep track of the time
    std::chrono::high_resolution_clock::time_point last_update_time_;

    // handle to the main executive runner thread
    std::atomic_bool running_;
    std::thread executive_handle_;
};

}
