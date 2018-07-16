#include "light_control/scene_helpers.hh"

namespace light_control
{

scene_helper& scene_helper::get_instance()
{
    static scene_helper instance;
    return instance;
}
}
