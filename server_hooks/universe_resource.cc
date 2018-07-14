#include <random>
#include "json/parser.hh"
#include "server_hooks/universe_resource.hh"

#include "logging.hh"

#include <chrono>

template <size_t STEPS>
void perform_fade(lights::abstract_light::ptr light, std::vector<uint8_t> new_values)
{
    constexpr auto WAIT = std::chrono::duration<double>{1E-2};

    const auto& old_values = light->get_channels();

    // Check if the thing really needs to fade, if the change is small, just jump right to it (for live updates)
    constexpr double FADE_THRESHOLD = 10;
    bool needs_to_fade = false;

    // Keep track of the channel values and the amount we should add to each channel for each update
    std::vector<double> channel_values;
    std::vector<double> units_per_step;
    for (size_t i = 0; i < new_values.size(); ++i)
    {
        const uint8_t old_value = old_values.at(i).level;

        double delta = new_values.at(i) - old_value;
        units_per_step.emplace_back(delta / STEPS);

        if (std::abs(delta) > FADE_THRESHOLD)
        {
            needs_to_fade = true;
        }

        channel_values.emplace_back(old_value);
    }

    if (needs_to_fade == false)
    {
        light->set_channels(new_values);
        return;
    }

    for (size_t i = 0; i < STEPS; ++i)
    {
        // We need to have a uint8_t vector to update with
        std::vector<uint8_t> to_update_with(channel_values.size());
        for (size_t c = 0; c < channel_values.size(); ++c)
        {
            channel_values[c] += units_per_step[c];
            to_update_with[c] = channel_values[c];
        }

        light->set_channels(to_update_with);
        std::this_thread::sleep_for(WAIT);
    }

    light->set_channels(new_values);
}

//
// ############################################################################
//

json::json universe_resource::get_json_resource()
{
    json::map_type json_universe_state;
    json::map_type json_lights;
    for (const auto& entry : light_map)
    {
        json::json json_light;
        json_light.set_map();

        json_light["light_name"] = std::string("test_name");

        json::vector_type json_channel_values;
        for (const auto& channel : entry.second->get_channels())
        {
            json_channel_values.emplace_back(static_cast<double>(channel.level));
        }
        json_light["channel_values"] = std::move(json_channel_values);

        json::vector_type json_channel_names;
        for (const auto& name : entry.second->get_channel_names())
        {
            json_channel_names.emplace_back(static_cast<std::string>(name));
        }
        json_light["channel_names"] = std::move(json_channel_names);

        json_lights[entry.first] = std::move(json_light);
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

    for (const std::pair<std::string, json::json>& entry : lights)
    {
        const std::string id = entry.first;

        auto light_found = light_map.find(id);
        if (light_found == light_map.end())
        {
            LOG_WARN("Got a JSON response with a light ID that wasn't found in the database.");
            continue;
        }
        lights::abstract_light::ptr light = light_found->second;

        json::vector_type channel_update = entry.second.get<json::vector_type>();
        const size_t expected_count = light->get_channel_count();

        if (channel_update.size() != expected_count)
        {
            LOG_WARN("JSON response has an invalid number of channels (" << expected_count << " != " << channel_update.size() << "). Ignoring.");
            continue;
        }

        std::vector<uint8_t> channel_update_processed;
        for (const json::json& v : channel_update)
        {
            channel_update_processed.emplace_back(v.get<double>());
        }

        perform_fade<20>(light, channel_update_processed);
    }

    return true;
}

//
// ############################################################################
//

static std::string generate_identifier()
{
    static uint16_t counter = 0;
    return std::to_string(counter++);
}

//
// ############################################################################
//

universe_resource_builder::universe_resource_builder(lights::light_universe_controller& controller_)
    : controller(controller_)
{
}

//
// ############################################################################
//

void universe_resource_builder::add_light_to_universe(lights::abstract_light::ptr light)
{
    std::string identifier = generate_identifier();
    building_map[generate_identifier()] = light;
    //controller.add_light_to_universe(std::move(light));
}

//
// ############################################################################
//

universe_resource universe_resource_builder::finalize()
{
    universe_resource resource;
    resource.light_map = std::move(building_map);
    return resource;
}
