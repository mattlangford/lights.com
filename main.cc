// For controlling lights
#include "light_control/light_universe_controller.hh"
#include "light_types/virtual_light.hh"
#include "serial_control/virtual_serial_interface.hh"

// For hosting webserver
#include "http_server/http_server.hh"
#include "resources/file_resource.hh"
#include "server_hooks/universe_resource.hh"
#include "json/json.hh"
#include "json/parser.hh"

#include <random>
#include <iostream>
#include <limits>
#include <map>
#include <memory>

constexpr auto ROOT_PATH = "/home/matt/Documents/dmx_control";

int main()
{
    auto light1 = std::make_shared<lights::virtual_light>(1, 5);
    auto light2 = std::make_shared<lights::virtual_light>(light1->get_end_address() + 1, 2);

    //
    // Configure the universe controller to not care what we do to it
    //
    lights::light_universe_controller::controller_params params;
    params.enforce_44hz = false;
    params.control = lights::light_universe_controller::control_type::MANUAL;

    serial::virtual_serial_interface interface;
    lights::light_universe_controller universe{interface, params};

    //
    // Build the universe using the resource builder
    //
    universe_resource_builder builder{universe};
    builder.add_light_to_universe(light1);
    builder.add_light_to_universe(light2);

    auto resource = std::make_shared<universe_resource>(builder.finalize());

    //
    // Make sure changing the lights themselves results in a change in the JSON output
    //
    light1->set_channel(2, 100);
    light2->set_channel(1, 77);
    std::cout << resource->get_json_resource().get_value_as_string() << "\n";

    //
    // Now spin a web server up, see if the client can access it
    //
    http_server server(8080);
    server.add_resource(resource);
    server.add_resource(std::make_shared<resources::file_resource>(std::string(ROOT_PATH) + "/server_hooks/test_server", "/index.html"));

    server.start_server();
    while (true)
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
}
