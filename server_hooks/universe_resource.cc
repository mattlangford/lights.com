#include <random>
#include "json/parser.hh"
#include "server_hooks/universe_resource.hh"

#include "logging.hh"

//
// ############################################################################
//

json::json universe_resource::get_json_resource()
{
    json::vector_type json_universe_state;
    for (const auto& entry : light_map)
    {
        json::json json_light;
        json_light.set_map();

        json_light["id"] = entry.first;
        json_light["state"] = entry.second->get_json_light_state();

        json_universe_state.emplace_back(std::move(json_light));
    }

    return json::json{json_universe_state};
}

//
// ############################################################################
//

bool universe_resource::handle_post_request(requests::POST post_request)
{
    json::json update = json::parse(post_request.post_data);

    for (const auto& entry : update.get<json::vector_type>())
    {
        const json::json& json_entry = entry;
        std::string id = json_entry["id"].get<std::string>();

        auto light_found = light_map.find(id);
        if (light_found == light_map.end())
        {
            LOG_WARN("Got a JSON response with a light ID that wasn't found in the database");
            continue;
        }

        light_found->second->set_json_light_state(json_entry["state"]);
    }

    return true;
}

//
// ############################################################################
//

static std::string generate_identifier()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<std::mt19937::result_type> dist(0, std::numeric_limits<uint16_t>::max());

    return std::to_string(dist(rng));
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
    controller.add_light_to_universe(std::move(light));
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
