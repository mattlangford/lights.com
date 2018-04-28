#pragma once
#include "light_types/abstract_light.hh"
#include "serial.hh"

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
    light_universe_controller(serial::serial_connection& connection, const controller_params& params);

    ~light_universe_controller();

public:
    // add a new light, return if the add was successful
    void add_light_to_universe(light_base::ptr light);

    // preform an update
    void do_update();

    // update the internal params
    void update_params(const controller_params& params);

private:
    // runs do_update every update_period seconds
    void executive_thread();

private:
    controller_params params_;

    // connection to the board that interfaces with the lights
    serial::serial_connection& connection_;

    // how long to sleep after each update
    std::chrono::duration<double> update_period_;

    // if we've been configured to enforce the 44hz DMX update rate, we'll need to keep track of the time
    std::chrono::high_resolution_clock::time_point last_update_time_;

    // all of the lights registered, each timestep will get
    std::vector<abtract_light::ptr> lights_;
    std::array<bool, dmx::MAX_NUM_CHANNELS> valid_addresses_;

    // handle to the main executive runner thread
    std::atomic_bool running_;
    std::thread executive_handle_;
};

}
