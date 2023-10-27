
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
        rgb(controller.channel(address + 1),
            controller.channel(address + 2),
            controller.channel(address + 3)) {
        controller.set_max_channel(address + 7);
    }

    Channel& brightness;
    RgbChannel rgb;
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
            rgb_[light] = new RgbChannel(
                    controller.channel(address + light * 3),
                    controller.channel(address + light * 3 + 1),
                    controller.channel(address + light * 3 + 2));
        }
    }
    ~WashLightBar52() {
        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            delete rgb_[light];
        }
    }

    Channel& brightness() { return brightness_; }
    RgbChannel& rgb(uint8_t index) { return *rgb_[index]; }

private:
    Channel& brightness_;
    RgbChannel* rgb_[NUM_LIGHTS];
};

class WashBarLight112 {
public:
    static constexpr uint8_t NUM_LIGHTS = 28;
    static constexpr uint8_t NUM_CHANNELS = 112;

    WashBarLight112(uint16_t address, DMXController& controller) {
        controller.set_max_channel(address + NUM_CHANNELS);

        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            rgb_[light] = new RgbChannel(
                    controller.channel(address + light * 4),
                    controller.channel(address + light * 4 + 1),
                    controller.channel(address + light * 4 + 2));
            whites_[light] = &controller.channel(address + light * 4 + 3);
        }
    }
    ~WashBarLight112() {
        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            delete whites_[light];
            delete rgb_[light];
        }
    }

    Channel& white(uint8_t index) { return *whites_[index]; }
    RgbChannel& rgb(uint8_t index) { return *rgb_[index]; }

private:
    Channel* whites_[NUM_LIGHTS];
    RgbChannel* rgb_[NUM_LIGHTS];
};
