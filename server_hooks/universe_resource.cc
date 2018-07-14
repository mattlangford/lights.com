#include <chrono>
#include <exception>
#include <random>

#include "json/parser.hh"
#include "server_hooks/universe_resource.hh"
#include "utils/universe_utilities.hh"

#include "logging.hh"

template <size_t STEPS>
static void perform_fade(const config::light& light, std::vector<dmx::channel_t>& channels, std::vector<uint8_t> new_values)
{
    constexpr auto WAIT = std::chrono::duration<double>{1E-2};

    // Check if the thing really needs to fade, if the change is small, just jump right to it (for live updates)
    constexpr double FADE_THRESHOLD = 20;
    bool needs_to_fade = false;

    // Keep track of the channel values and the amount we should add to each channel for each update
    std::vector<double> channel_values;
    std::vector<double> units_per_step;
    for (size_t i = 0; i < new_values.size(); ++i)
    {
        const uint8_t old_value = channels.at(light.starting_address + i).level;

        double delta = new_values[i] - old_value;
        units_per_step.emplace_back(delta / STEPS);

        if (std::abs(delta) > FADE_THRESHOLD)
        {
            needs_to_fade = true;
        }

        channel_values.emplace_back(old_value);
    }

    if (needs_to_fade == false)
    {
        for (size_t i = 0; i < new_values.size(); ++i)
        {
            channels.at(light.starting_address + i).level = new_values[i];
        }
        return;
    }

    for (size_t i = 0; i < STEPS; ++i)
    {
        std::vector<uint8_t> to_update_with(channel_values.size());
        for (size_t c = 0; c < channel_values.size(); ++c)
        {
            channels[light.starting_address + c].level += units_per_step[c];
        }

        std::this_thread::sleep_for(WAIT);
    }

    // Make sure we get to the end values
    for (size_t i = 0; i < new_values.size(); ++i)
    {
        channels.at(light.starting_address + i).level = new_values[i];
    }
}

namespace server_hooks
{

universe_resource::universe_resource(const config::universe& universe, std::shared_ptr<std::vector<dmx::channel_t>> channels)
    : universe_(universe),
      channels_(channels)
{
    const size_t num_configured_channels = utils::get_num_channels(universe_);
    if (num_configured_channels != channels_->back().address)
    {
        LOG_ERROR("Number of configured channels doesn't match number of channels. ("
                << num_configured_channels << " != " << channels_->back().address << ")");

        throw std::runtime_error("Number of configured channels doesn't match number of channels.");
    }
}

//
// ############################################################################
//

json::json universe_resource::get_json_resource()
{
    json::map_type json_universe_state;
    json::vector_type json_lights;
    for (const config::light& light : universe_.lights)
    {
        json::json json_light;
        json_light.set_map();

        json_light["light_name"] = std::string(light.name);

        json::vector_type json_channels;

        const uint16_t base_address = light.starting_address;
        for (const config::channel& channel : light.channels)
        {
            uint16_t channel_address = base_address + channel.base_offset;

            json::map_type json_channel;
            json_channel["name"] = std::string(channel.name);
            json_channel["min_value"] = (double)channel.min_value;
            json_channel["max_value"] = (double)channel.max_value;
            json_channel["level"] = (double)channels_->at(channel_address).level;

            json_channels.emplace_back(std::move(json_channel));
        }
        json_light["channels"] = std::move(json_channels);
        json_lights.emplace_back(std::move(json_light));
    }
    json_universe_state["lights"] = std::move(json_lights);

    LOG_DEBUG("Got lights");
    return json::json{json_universe_state};
}

//
// ############################################################################
//

bool universe_resource::handle_post_request(requests::POST post_request)
{
    json::map_type update = json::parse(post_request.post_data).get<json::map_type>();

    json::map_type lights = update["lights"].get<json::map_type>();

    for (const std::pair<std::string, json::json>& entry : lights)
    {
        const size_t light_index = std::stoi(entry.first);

        const config::light& this_light = universe_.lights[light_index];

        std::vector<uint8_t> to_update;
        for (const json::json& v : lights["channel_values"].get<json::vector_type>())
        {
            to_update.emplace_back(v.get<double>());
        }

        perform_fade<20>(this_light, *channels_, to_update);
    }

    return true;
}
}
