#include "light_types/virtual_light.hh"

namespace lights
{

//
// ############################################################################
//

virtual_light::virtual_light(const uint8_t start_address, const uint8_t num_channels)
    : channels(num_channels)
{
    uint8_t address = start_address;
    for (auto& channel : channels)
    {
        channel.address = ++address;
        channel.level = 0;
    }
}

//
// ############################################################################
//

void virtual_light::set_channel(const uint8_t channel, const uint8_t level)
{
    for (auto& this_channel : channels)
    {
        if (this_channel.address == channel)
        {
            this_channel.level = level;
            return;
        }
    }
}

//
// ############################################################################
//

void virtual_light::set_off()
{
    for (auto& this_channel : channels)
    {
        this_channel.level = 0;
    }
}

//
// ############################################################################
//

json::json virtual_light::get_json_light_state() const
{
    json::vector_type this_light;
    for (const auto& this_channel : channels)
    {
        this_light.push_back(json::json{static_cast<double>(this_channel.level)});
    }

    return json::json{this_light};
}

//
// ############################################################################
//

void virtual_light::set_json_light_state(const json::json& j)
{
    json::vector_type desired_channels = j.get<json::vector_type>();
    if (desired_channels.size() != channels.size())
    {
        std::cout << "Invalid light state size!\n";
        return;
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        channels[i].level = desired_channels[i].get<double>();
    }
}
}
