// For controlling lights
#include "config/universe_generators.hh"
#include "light_control/dmx.hh"
#include "light_control/light_universe_controller.hh"
#include "serial_control/ftd2xx_serial_interface.hh"
#include "serial_control/virtual_serial_interface.hh"

// For loading scenes
#include "light_control/scene_helpers.hh"

#include "logging.hh"

#include <string>


bool try_to_get_scene_name(int argc, char** argv, std::string& scene)
{
    constexpr int REQ_NUM_ARGS = 2;
    if (argc < REQ_NUM_ARGS)
    {
        LOG_ERROR("Not enough arguments! Required to have 1: scene_name");
        return false;
    }

    scene = argv[1];
    return true;
}

int main(int argc, char** argv)
{
    std::string scene_name;
    if (try_to_get_scene_name(argc, argv, scene_name) == false)
    {
        LOG_ERROR("Unable to get scene name!");
        return 1;
    }
    LOG_DEBUG("Loading scene \"" << scene_name << "\".");

    light_control::abstract_scene_ptr scene
        = light_control::scene_helper::get_instance().get_scene(scene_name);

    if (scene == nullptr)
    {
        LOG_ERROR("Scene \"" << scene_name << "\" could not be found. Make sure it's been registered properly.");
        return 1;
    }
    LOG_DEBUG("Scene: \"" << scene_name << "\" loaded successfully!");

    const config::universe universe_config = config::generate_living_room_universe();

    LOG_DEBUG("Universe configuration loaded.")

    //
    // Configure the universe controller and serial interface
    //
    light_control::light_universe_controller::controller_params params;
    params.control = light_control::light_universe_controller::control_type::EXECUTIVE_AUTO;

    serial::ftd2xx_serial_interface interface(dmx::BAUDRATE);
    //serial::virtual_serial_interface interface;
    light_control::light_universe_controller universe{interface, params, universe_config};

    universe.get_scheduler().enqueue_entries(scene->get_schedule(universe_config));

    LOG_DEBUG("Universe controller constructed.")

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
    }
}
