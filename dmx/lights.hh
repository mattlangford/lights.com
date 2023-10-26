
#include "dmx.hh"
#include "effects.hh"
#include "util.hh"

struct RgbChannel {
public:
    RgbChannel(Channel& red, Channel& green, Channel& blue) : red_(red), green_(green), blue_(blue) { }

    template <typename Effect>
    void add_effect() {
        red_.add_effect(new Effect());
        green_.add_effect(new Effect());
        blue_.add_effect(new Effect());
    }

    RgbEffect effect(size_t index) {
        return RgbEffect(red_.effect(index), green_.effect(index), blue_.effect(index));
    }

private:
    Channel& red_;
    Channel& green_;
    Channel& blue_;
};

/*
   uint8_t lights_aurora[kTotalSlots] = {
     255, 255, 15, 70, 0, 0, 0,
     255, 50, 90, 200, 0, 0, 0,
     255, 255, 30, 50, 0, 0, 0,
     255, 30, 50, 200, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0
   };
*/

class KitchenLight {
public:
    KitchenLight(uint16_t address, DMXController& controller) :
        brightness(controller.channel(address)),
        red(controller.channel(address + 1)),
        green(controller.channel(address + 2)),
        blue(controller.channel(address + 3)) {
        controller.set_max_channel(address + 6);
    }

public:
    Channel& brightness;
    Channel& red;
    Channel& green;
    Channel& blue;
};


/*
class WashLightBar52 {
public:
    static constexpr uint8_t NUM_LIGHTS = 16;
    static constexpr uint8_t NUM_CHANNELS = 52;

    WashLightBar52(uint8_t address, DMXController& controller) {
        controller.set_max_channel(address + NUM_CHANNELS);

        size_t channel = 0;
        controller.add_channel(address++, channels[channel++]);
        controller.add_channel(address++, bonus_channels[0]);
        for (uint8_t light = 0; light < NUM_LIGHTS; ++light) {
            auto& red = channels[channel++];
            auto& green = channels[channel++];
            auto& blue = channels[channel++];

            rgb[light].set_red(&red);
            controller.add_channel(address++, red);
            rgb[light].set_green(&green);
            controller.add_channel(address++, green);
            rgb[light].set_blue(&blue);
            controller.add_channel(address++, blue);
        }

        controller.add_channel(address++, bonus_channels[1]);
        controller.add_channel(address++, bonus_channels[2]);
    }

    void set_goal(uint8_t r, uint8_t g, uint8_t b, uint32_t duration_ms=0) {
        for (uint8_t light = 0; light < NUM_LIGHTS; ++light) {
            rgb[light].set_goal(r, g, b, duration_ms);
        }
    }

    LinearFadeChannel& brightness() {
        return channels[0];
    }

    LinearFadeChannel channels[3 * NUM_LIGHTS + 1];
    LinearFadeChannel bonus_channels[3];
    RgbChannel<LinearFadeChannel> rgb[NUM_LIGHTS];
};

*/
