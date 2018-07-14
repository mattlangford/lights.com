#include "config/light.hh"
#include "config/channel.hh"

static config::light get_test_light()
{
    config::light test_light;
    test_light.starting_address = 55;

    config::channel red_channel;
    red_channel.name = "red";
    red_channel.base_offset = 0;
    red_channel.min_value = 0;
    red_channel.max_value = 255;
    red_channel.role = config::channel_role::RED;
    test_light.channels.emplace_back(std::move(red_channel));

    config::channel blue_channel;
    blue_channel.name = "blue";
    blue_channel.base_offset = 1;
    blue_channel.min_value = 0;
    blue_channel.max_value = 255;
    blue_channel.role = config::channel_role::BLUE;
    test_light.channels.emplace_back(std::move(blue_channel));

    return test_light;
}

int main()
{
    std::cout << get_test_light() << "\n";
}
