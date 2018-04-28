#include "litake_basic_light.hh"
#include <iostream>

namespace lights
{

litake_basic_light::litake_basic_light(size_t start_address)
    : start_address_(start_address), mod_state_value_(255)
{
}

//
// ############################################################################
//

void litake_basic_light::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t color = 0;

    color += blue;
    color <<= 8;
    color += green;
    color <<= 8;
    color += red;

    light_state_.store(color, std::memory_order_relaxed);
}

//
// ############################################################################
//

void litake_basic_light::set_off()
{
    mod_state_value_.store(0, std::memory_order_relaxed);
}

//
// ############################################################################
//

void litake_basic_light::set_flash(uint8_t flash_speed)
{
    uint8_t value_to_set = std::min<uint8_t>(1, std::max<uint8_t>(value_to_set, 127)) + 128;
    mod_state_value_.store(value_to_set, std::memory_order_relaxed);
}

//
// ############################################################################
//

void litake_basic_light::set_brightness(uint8_t brightness)
{
    uint8_t value_to_set = std::min<uint8_t>(0, std::max<uint8_t>(brightness, 127));
    mod_state_value_.store(value_to_set, std::memory_order_relaxed);
}

//
// ############################################################################
//

std::vector<dmx::dmx_helper::channel_t> litake_basic_light::get_channels()
{
    std::vector<dmx::dmx_helper::channel_t> channels(NUM_CHANNELS);
    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        channels[channel].address = get_start_address() + channel;
    }

    channels[0].level = mod_state_value_.load(std::memory_order_relaxed);

    uint32_t light_state = light_state_.load(std::memory_order_relaxed);
    channels[1].level = 0xFF & light_state;
    light_state >>= 8;
    channels[2].level = 0xFF & light_state;
    light_state >>= 8;
    channels[3].level = 0xFF & light_state;

    return channels;
}

}
