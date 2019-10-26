#include "config/light_generators.hh"
#include "config/channel_roles.hh"
#include "config/channel.hh"

namespace config
{

light generate_litake_basic_light(std::string light_name, uint16_t& address)
{
    light l;
    l.name = std::move(light_name);
    l.starting_address = address;

    config::channel brightness;
    brightness.name = "Brightness";
    brightness.base_offset = 0;
    brightness.min_value = 0;
    brightness.max_value = 127;
    brightness.role = channel_role::BRIGHTNESS;
    l.channels.emplace_back(std::move(brightness));

    // TODO: Figure out how to handle channels with multiple ranges
    // config::channel flash;
    // flash.name = "Flash Speed";
    // flash.base_offset = 0;
    // flash.min_value = 128;
    // flash.max_value = 255;
    // flash.role = channel_role::FLASH;
    // l.channels.emplace_back(std::move(flash));
    address++;

    config::channel red;
    red.name = "Red";
    red.base_offset = 1;
    red.min_value = 0;
    red.max_value = 255;
    red.role = channel_role::RED;
    l.channels.emplace_back(std::move(red));
    address++;

    config::channel green;
    green.name = "Green";
    green.base_offset = 2;
    green.min_value = 0;
    green.max_value = 255;
    green.role = channel_role::GREEN;
    l.channels.emplace_back(std::move(green));
    address++;

    config::channel blue;
    blue.name = "Blue";
    blue.base_offset = 3;
    blue.min_value = 0;
    blue.max_value = 255;
    blue.role = channel_role::BLUE;
    l.channels.emplace_back(std::move(blue));
    address++;

    return l;
}

//
// ############################################################################
//

light generate_lm70_moving_light(std::string light_name, uint16_t& address)
{
    // This is for 9 channel mode
    light l;
    l.name = std::move(light_name);
    l.starting_address = address;

    config::channel pan;
    pan.name = "Pan";
    pan.base_offset = 1;
    pan.min_value = 0;
    pan.max_value = 255;
    pan.role = channel_role::PAN;
    l.channels.emplace_back(std::move(pan));
    address++;

    config::channel tilt;
    tilt.name = "Tilt";
    tilt.base_offset = 2;
    tilt.min_value = 0;
    tilt.max_value = 255;
    tilt.role = channel_role::TILT;
    l.channels.emplace_back(std::move(tilt));
    address++;

    config::channel brightness;
    brightness.name = "Brightness";
    brightness.base_offset = 3;
    brightness.min_value = 8;
    brightness.max_value = 135;
    brightness.role = channel_role::BRIGHTNESS;
    l.channels.emplace_back(std::move(brightness));
    address++;

    config::channel red;
    red.name = "Red";
    red.base_offset = 7;
    red.min_value = 0;
    red.max_value = 255;
    red.role = channel_role::RED;
    l.channels.emplace_back(std::move(red));
    address++;

    config::channel green;
    green.name = "Green";
    green.base_offset = 8;
    green.min_value = 0;
    green.max_value = 255;
    green.role = channel_role::GREEN;
    l.channels.emplace_back(std::move(green));
    address++;

    config::channel blue;
    blue.name = "Blue";
    blue.base_offset = 9;
    blue.min_value = 0;
    blue.max_value = 255;
    blue.role = channel_role::BLUE;
    l.channels.emplace_back(std::move(blue));
    address++;

    config::channel white;
    white.name = "White";
    white.base_offset = 7;
    white.min_value = 0;
    white.max_value = 255;
    white.role = channel_role::WHITE;
    l.channels.emplace_back(std::move(white));
    address++;

    // TODO: Speed
    address++;

    // TODO: Reset
    address++;

    return l;
}

//
// ############################################################################
//

light generate_misseye_light(std::string light_name, uint16_t& address)
{
    light l;
    l.name = std::move(light_name);
    l.starting_address = address;

    config::channel brightness;
    brightness.name = "Brightness";
    brightness.base_offset = 0;
    brightness.min_value = 0;
    brightness.max_value = 255;
    brightness.role = channel_role::BRIGHTNESS;
    l.channels.emplace_back(std::move(brightness));
    address++;

    config::channel red;
    red.name = "Red";
    red.base_offset = 1;
    red.min_value = 0;
    red.max_value = 255;
    red.role = channel_role::RED;
    l.channels.emplace_back(std::move(red));
    address++;

    config::channel green;
    green.name = "Green";
    green.base_offset = 2;
    green.min_value = 0;
    green.max_value = 255;
    green.role = channel_role::GREEN;
    l.channels.emplace_back(std::move(green));
    address++;

    config::channel blue;
    blue.name = "Blue";
    blue.base_offset = 3;
    blue.min_value = 0;
    blue.max_value = 255;
    blue.role = channel_role::BLUE;
    l.channels.emplace_back(std::move(blue));
    address++;

    // TODO: Strobe
    address++;

    // TODO: Color stuff?
    address++;

    // TODO: I don't know
    address++;

    return l;
}
}
