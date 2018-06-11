#include "json/json.hh"
#include "json/parser.hh"
#include "server_hooks/universe_resource.hh"
#include "light_control/light_universe_controller.hh"
#include "light_types/virtual_light.hh"

#include <random>
#include <iostream>
#include <limits>
#include <map>


int main()
{
    lights::virtual_light light{1, 5};

    lights::light_universe_controller universe;


}
