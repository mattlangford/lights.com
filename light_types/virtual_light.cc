#include "light_types/virtual_light.hh"
#include <iostream>

namespace lights
{

//
// ############################################################################
//

virtual_light::virtual_light(const uint8_t start_address, const uint8_t num_channels)
    : channels_(num_channels), channel_names_(num_channels)
{
    for (size_t i = 0; i < num_channels; ++i)
    {
        dmx::dmx_helper::channel_t& channel = channels_.at(i);
        std::string& channel_name = channel_names_.at(i);

        channel.address = i + start_address;
        channel.level = 0;

        channel_name = "channel_" + std::to_string(i);
    }
}

//
// ############################################################################
//

void virtual_light::set_channel(const uint8_t channel, const uint8_t level)
{
    channels_.at(channel).level = level;
}

//
// ############################################################################
//

void virtual_light::set_off()
{
    for (auto& this_channel : channels_)
    {
        this_channel.level = 0;
    }
}

//
// ############################################################################
//

void virtual_light::set_channels(std::vector<uint8_t> channels)
{
    if (channels.size() != channels_.size())
    {
        std::cout << "Trying to set channels with an invalid length vector!\n";
        return;
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        channels_.at(i).level = channels.at(i);
    }
}
}
