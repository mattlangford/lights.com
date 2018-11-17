#include <chrono>
#include <exception>
#include <random>

#include "json/parser.hh"
#include "server_hooks/universe_resource.hh"
#include "light_control/scene_helpers.hh"
#include "utils/universe_utilities.hh"

#include "logging.hh"

namespace
{
std::string read_json_object(server::tcp_message& message)
{
    std::string read_data;
    size_t count = 0;
    while (count > 0 || read_data.empty()) // TODO timeout
    {
        read_data.resize(read_data.size() + 1);
        uint8_t* message_dest = reinterpret_cast<uint8_t*>(&read_data.back());

        message.read_into(message_dest, 1);
        if (*message_dest == '{')
        {
            count++;
        }
        else if (*message_dest == '}')
        {
            count--;
        }
    }

    return read_data;
}
}

namespace server_hooks
{

universe_resource::universe_resource(const config::universe& universe,
                                     light_control::light_universe_controller& controller)
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

    json::vector_type scenes;
    for (const std::string& scene_name : light_control::scene_helper::get_instance().get_registered_scenes())
    {
        scenes.emplace_back(static_cast<std::string>(scene_name));
    }
    json_universe_state["scenes"] = std::move(scenes);

    return json::json{json_universe_state};
}

//
// ############################################################################
//

bool universe_resource::handle_post_request(requests::POST post_request)
{
    std::string str_json_object = read_json_object(post_request.tcp_connection);
    json::map_type update = json::parse(std::move(str_json_object)).get<json::map_type>();

    //
    // This update may have a couple different things in it. First we want to check if the user requested to activate
    // any scenes, then if not, we can check what the light state they want to set is
    //
    auto scene_request = update.find("scene");
    if (scene_request != update.end())
    {
        // they requested a scene!
        const std::string& scene_name = scene_request->second.get<std::string>();
        auto scene = light_control::scene_helper::get_instance().get_scene(scene_name);
        LOG_DEBUG("scene_request: " << scene_name << ", " << scene);
        if (scene == nullptr)
        {
            LOG_ERROR("No scene found called: " << scene_name);
            return false;
        }
        constexpr bool preempt = true;
        controller_.get_scheduler().enqueue_entries(scene->get_schedule(universe_), preempt);
        return true;
    }

    // they must have asked to set some lights
    auto light_request = update.find("lights");
    if (light_request == update.end())
    {
        LOG_ERROR("No scene or lights found in request! Ignoring...");
        return false;
    }

    json::map_type lights = light_request->second.get<json::map_type>();

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
            throw std::runtime_error("No transition_duration found in update even though transition_type was"
                                     "exponential fade");
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
