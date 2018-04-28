#include "lights/litake_basic_light.hh"
#include "light_universe_controller.hh"
#include "dmx.hh"
#include "serial.hh"

#include <iostream>
#include <thread>

struct Color
{
    Color(double r_, double g_, double b_)
        : r(r_), g(g_), b(b_)
    {
    }

    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
};

Color do_fade(const Color& start, const Color& end, double amount)
{
    const double r_component = end.r - start.r;
    const double g_component = end.g - start.g;
    const double b_component = end.b - start.b;

    return {start.r + amount * r_component,
            start.g + amount * g_component,
            start.b + amount * b_component};
}

int main()
{
    Color end {255, 197, 143};
    Color start {255.0, 255.0, 200.0};

    constexpr size_t START_ADDRESS = 1;
    lights::litake_basic_light::ptr light = std::make_shared<lights::litake_basic_light>(START_ADDRESS);

    serial::serial_connection connection(dmx::BAUDRATE);

    lights::light_universe_controller::controller_params params;
    params.control = lights::light_universe_controller::control_type::MANUAL;
    params.enforce_44hz = true;
    lights::light_universe_controller universe(connection, params);

    universe.add_light_to_universe(light);

    light->set_color(start.r, start.g, start.b);
    double fade = 0.0;
    while (fade < 1.0)
    {
        Color c = do_fade(start, end, fade);
        fade += 0.01;

        light->set_color(c.r, c.g, c.b);
        universe.do_update();
    }

    light->set_color(end.r, end.g, end.b);
    while (true)
    {
        universe.do_update();
    }
}
