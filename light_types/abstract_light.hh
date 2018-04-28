#pragma once

#include <memory>

#include "../dmx.hh"

//
// Base light, allows the light manager to get channels from each light and
// make sure that the lights don't conflict with each other.
//
// NOTE: It's important that each light is THREAD SAFE, since there will be
// a thread that will be polling it
//

namespace lights
{

class light_base
{
public: // types //////////////////////////////////////////////////////////////
    typedef std::shared_ptr<light_base> ptr;

public: // destructor /////////////////////////////////////////////////////////
    virtual ~light_base() = default;

public: // methods ////////////////////////////////////////////////////////////
    // get the start and end DMX address for this light, used to make sure
    // no conflicts exist
    virtual size_t get_start_address() const = 0;
    virtual size_t get_end_address() const = 0;

    // get the channels this light has to offer
    virtual std::vector<dmx::dmx_helper::channel_t> get_channels() = 0;

    // every light should be able to turn off
    virtual void set_off() = 0;
};
}
