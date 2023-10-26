
#include "dmx.hh"
#include "effects.hh"
#include "util.hh"

struct RgbChannel {
public:
    static RgbChannel create(uint16_t address, DMXController controller) {
        return RgbChannel(
            &controller.channel(address),
            &controller.channel(address + 1),
            &controller.channel(address + 2));
    }

    RgbChannel(Channel* red=nullptr, Channel* green=nullptr, Channel* blue=nullptr) :
        red_(red), green_(green), blue_(blue) {
    }

    template <typename Effect, typename...Args>
    RgbEffect add_effect(Args...args) {
        Effect* r = new Effect(args...);
        Effect* g = new Effect(args...);
        Effect* b = new Effect(args...);

        if (red_) { red_->add_effect(r); }
        if (green_) { green_->add_effect(g); }
        if (blue_) { blue_->add_effect(b); }

        return RgbEffect(r, g, b);
    }

    RgbEffect effect(size_t index) {
        return RgbEffect(
            red_ ? red_->effect(index) : nullptr,
            green_ ? green_->effect(index) : nullptr,
            blue_ ? blue_->effect(index) : nullptr);
    }

private:
    Channel* red_ = nullptr;
    Channel* green_ = nullptr;
    Channel* blue_ = nullptr;
};


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
        rgb(RgbChannel::create(address + 1, controller)) {
        controller.set_max_channel(address + 7);
    }

public:
    Channel& brightness;
    RgbChannel rgb;
};


class WashLightBar52 {
public:
    static constexpr uint8_t NUM_LIGHTS = 16;
    static constexpr uint8_t NUM_CHANNELS = 52;

    WashLightBar52(uint16_t address, DMXController& controller) :
        brightness(controller.channel(address++)) {
        controller.set_max_channel(address + NUM_CHANNELS);

        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            rgb[light] = RgbChannel::create(address + light * 3, controller);
        }
    }

    Channel& brightness;
    RgbChannel rgb[NUM_LIGHTS];
};

class WashBarLight112 {
public:
    static constexpr uint8_t NUM_LIGHTS = 28;
    static constexpr uint8_t NUM_CHANNELS = 112;

    WashBarLight112(uint16_t address, DMXController& controller) {
        controller.set_max_channel(address + NUM_CHANNELS);

        for (size_t light = 0; light < NUM_LIGHTS; ++light) {
            // Skip over white
            rgb[light] = RgbChannel::create(address + light * 4, controller);
            whites[light] = &controller.channel(light * 4 + 3);
        }
    }

    Channel* whites[NUM_LIGHTS];
    RgbChannel rgb[NUM_LIGHTS];
};
