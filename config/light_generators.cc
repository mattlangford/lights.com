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
    address++;

    config::channel flash;
    flash.name = "Flash Speed";
    flash.base_offset = 0;
    flash.min_value = 128;
    flash.max_value = 255;
    flash.role = channel_role::FLASH;
    l.channels.emplace_back(std::move(flash));
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

}
