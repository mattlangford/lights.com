#pragma once

#include "dmx.hh"
#include "effects.hh"

class Missyee36Light {
public:
    Missyee36Light(uint16_t address, DMXController& controller) :
        brightness(controller.channel(address)),
        red(controller.channel(address + 1)),
        green(controller.channel(address + 2)),
        blue(controller.channel(address + 3)) {
        controller.set_max_channel(address + 7);
    }

    Channel& brightness;
    Channel& red;
    Channel& green;
    Channel& blue;
};

class Electro86Light {
public:
    Electro86Light(uint16_t address, DMXController& controller) :
        red(controller.channel(address + 0)),
        green(controller.channel(address + 1)),
        blue(controller.channel(address + 2)) {

        // Max brightness
        controller.channel(address + 3).set_value(189);
    }

    Channel& red;
    Channel& green;
    Channel& blue;
};

class BetopperMover9Light {
public:
    BetopperMover9Light(uint16_t address, DMXController& controller) :
        x(controller.channel(address + 0)),
        y(controller.channel(address + 1)),
        red(controller.channel(address + 3)),
        green(controller.channel(address + 4)),
        blue(controller.channel(address + 5)),
        white(controller.channel(address + 6)),
        speed(controller.channel(address + 7)) {

        // Max brightness
        controller.channel(address + 2).set_value(135);
        controller.set_max_channel(address + 9);
    }

    Channel& x;
    Channel& y;

    Channel& red;
    Channel& green;
    Channel& blue;
    Channel& white;

    Channel& speed;
};

class WashBarLight112 {
public:
    static constexpr uint8_t NUM_LIGHTS = 28;
    static constexpr uint8_t NUM_CHANNELS = 112;

    WashBarLight112(uint16_t address, DMXController& controller) {
        controller.set_max_channel(address + NUM_CHANNELS);

        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            rgb_[light].r = &controller.channel(address + light * 4);
            rgb_[light].g = &controller.channel(address + light * 4 + 1);
            rgb_[light].b = &controller.channel(address + light * 4 + 2);
            rgb_[light].w = &controller.channel(address + light * 4 + 3);
        }
    }

    Channel& red(uint8_t index) { return *rgb_[index].r; }
    Channel& green(uint8_t index) { return *rgb_[index].g; }
    Channel& blue(uint8_t index) { return *rgb_[index].b; }
    Channel& white(uint8_t index) { return *rgb_[index].w; }

private:
    struct Rgbw {
        Channel* r;
        Channel* g;
        Channel* b;
        Channel* w;
    };
    Rgbw rgb_[NUM_LIGHTS];
};
