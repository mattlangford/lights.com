#pragma once

#include "dmx.hh"
#include "effects.hh"
#include "util.hh"

// Not sure of the brand of this light, this is all I have to go off of:
//   uint8_t lights_aurora[kTotalSlots] = {
//     255, 255, 15, 70, 0, 0, 0,
//     255, 50, 90, 200, 0, 0, 0,
//     255, 255, 30, 50, 0, 0, 0,
//     255, 30, 50, 200, 0, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//   };
class KitchenLight {
public:
    KitchenLight(uint16_t address, DMXController& controller) :
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


class WashLightBar52 {
public:
    static constexpr uint8_t NUM_LIGHTS = 16;
    static constexpr uint8_t NUM_CHANNELS = 52;

    WashLightBar52(uint16_t address, DMXController& controller) :
        brightness_(controller.channel(address++)) {
        controller.set_max_channel(address + NUM_CHANNELS);
        address++;

        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            rgb_[light].r = &controller.channel(address + light * 3);
            rgb_[light].g = &controller.channel(address + light * 3 + 1);
            rgb_[light].b = &controller.channel(address + light * 3 + 2);
        }
    }

    Channel& brightness() { return brightness_; }
    Channel& red(uint8_t index) { return *rgb_[index].r; }
    Channel& green(uint8_t index) { return *rgb_[index].g; }
    Channel& blue(uint8_t index) { return *rgb_[index].b; }

private:
    Channel& brightness_;

    struct Rgb {
        Channel* r;
        Channel* g;
        Channel* b;
    };
    Rgb rgb_[NUM_LIGHTS];
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
