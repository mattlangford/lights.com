#include <chrono>
#include <exception>
#include <random>

#include "json/parser.hh"
#include "server_hooks/universe_resource.hh"
#include "utils/universe_utilities.hh"

#include "logging.hh"

namespace server_hooks
{

universe_resource::universe_resource(const config::universe& universe, light_control::light_universe_controller& controller)
    : universe_(universe),
      controller_(controller)
{
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
            json_channel["base_offset"] = (double)channel.base_offset;
            json_channel["min_value"] = (double)channel.min_value;
            json_channel["max_value"] = (double)channel.max_value;
            json_channel["level"] = (double)controller_.get_channels().at(channel_address).level;

            json_channels.emplace_back(std::move(json_channel));
        }
        json_light["channels"] = std::move(json_channels);
        json_lights.emplace_back(std::move(json_light));
    }
    json_universe_state["lights"] = std::move(json_lights);

    return json::json{json_universe_state};
}

//
// ############################################################################
//

bool universe_resource::handle_post_request(requests::POST post_request)
{
    json::map_type update = json::parse(post_request.post_data).get<json::map_type>();

    json::map_type lights = update["lights"].get<json::map_type>();

    std::vector<dmx::channel_t> to_update;
    for (const std::pair<std::string, json::json>& entry : lights)
    {
        const size_t light_index = std::stoi(entry.first);

        const config::light& this_light = universe_.lights[light_index];

        auto update_values = entry.second.get<json::vector_type>();
        for (size_t i = 0; i < update_values.size(); ++i)
        {
            dmx::channel_t channel;
            channel.address = this_light.starting_address + i;
            channel.level = update_values[i].get<double>();
            to_update.emplace_back(std::move(channel));
        }
    }

    light_control::schedule_entry entry;
    entry.transition_to = std::move(to_update);

    auto transition_type = update.find("transition_type");
    if (transition_type == update.end())
    {
        LOG_ERROR("No transition_type found in update");
        throw std::runtime_error("No transition_type found in update");
    }

    if (transition_type->second.get<std::string>() == "exponential_fade")
    {
        auto transition_duration = update.find("transition_duration");
        if (transition_duration == update.end())
        {
            LOG_ERROR("No transition_duration found in update even though transition_type was exponential fade");
            throw std::runtime_error("No transition_duration found in update even though transition_type was exponential fade");
        }

        entry.transition_type = light_control::transition_type_t::EXPONENTIAL_FADE;
        entry.transition_duration_s = transition_duration->second.get<double>();
    }
    else // if (transition_type == "direct")
    {
        entry.transition_type = light_control::transition_type_t::DIRECT;
        entry.transition_duration_s = 0.0;
    }

    controller_.get_scheduler().enqueue_entry(std::move(entry));

    return true;
}
}
