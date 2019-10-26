#include "config/universe_generators.hh"
#include "config/light_generators.hh"

namespace config
{

universe generate_living_room_universe()
{
    uint16_t address = 1;

    universe living_room;
    living_room.lights.emplace_back(generate_litake_basic_light("Right Side", address));
    living_room.lights.emplace_back(generate_litake_basic_light("Left Side", address));
    living_room.lights.emplace_back(generate_misseye_light("TODO", address));
    living_room.lights.emplace_back(generate_misseye_light("TODO1", address));
    living_room.lights.emplace_back(generate_misseye_light("TODO2", address));
    living_room.lights.emplace_back(generate_misseye_light("TODO3", address));
    living_room.lights.emplace_back(generate_lm70_moving_light("Mover", address));

    return living_room;
}
}
