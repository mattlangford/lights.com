#pragma once

#include "dmx.hh"
#include "effects.hh"

class RgbFixture {
public:
    RgbFixture(Channel& red, Channel& green, Channel& blue) :
        red_(red), green_(green), blue_(blue) {
    }

    virtual ~RgbFixture() = default;

    template <typename Effect>
    void add_effect(Effect& effect) {
        red_.add_effect(&effect);
        green_.add_effect(&effect);
        blue_.add_effect(&effect);
    }
    template <typename Effect>
    void add_rgb_effect(RgbEffect<Effect>& effect) {
        red_.add_effect(&effect.red());
        green_.add_effect(&effect.green());
        blue_.add_effect(&effect.blue());
    }

    // Copied from ChatGPT
    void set_hsv(float h, float s, float v) {
        // Convert HSV values to the range [0, 1]
        float h_normalized = h / 255.0;
        float s_normalized = s / 255.0;

        int i = static_cast<int>(h_normalized * 6);
        float f = (h_normalized * 6) - i;

        // Each of these will be between 0 and 255
        float p = static_cast<uint8_t>(v * (1 - s_normalized));
        float q = static_cast<uint8_t>(v * (1 - f * s_normalized));
        float t = static_cast<uint8_t>(v * (1 - (1 - f) * s_normalized));

        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        switch (i % 6) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
            default: r = 0; g = 0; b = 0;
        }

        set_rgb(r, g, b);
    }

    void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
        red_.set_value(r);
        green_.set_value(g);
        blue_.set_value(b);
    }

    Channel& red() { return red_; }
    Channel& green() { return green_; }
    Channel& blue() { return blue_; }

private:
    Channel& red_;
    Channel& green_;
    Channel& blue_;
};

class MissyeeLight : public RgbFixture {
public:
    MissyeeLight(uint16_t address, DMXController& controller) :
        RgbFixture(
            controller.channel(address + 1),
            controller.channel(address + 2),
            controller.channel(address + 3)),
        brightness(controller.channel(address)) {

        brightness.set_value(255);
        controller.set_max_channel(address + 7);
    }

    Channel& brightness;
};

class LitakeLight : public RgbFixture {
public:
    LitakeLight(uint16_t address, DMXController& controller) :
        RgbFixture(
            controller.channel(address + 1),
            controller.channel(address + 2),
            controller.channel(address + 3)) {

        // Max brightness.
        controller.channel(address).set_value(127);
    }
};

class BetopperMover9Light : public RgbFixture {
public:
    BetopperMover9Light(uint16_t address, DMXController& controller) :
        RgbFixture(
            controller.channel(address + 3),
            controller.channel(address + 4),
            controller.channel(address + 5)),
        x(controller.channel(address + 0)),
        y(controller.channel(address + 1)),
        white(controller.channel(address + 6)),
        speed(controller.channel(address + 7)) {

        // Max brightness
        controller.channel(address + 2).set_value(255);
        controller.set_max_channel(address + 9);
    }

    Channel& x;
    Channel& y;

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

    void set_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white=0) {
        for (auto& rgb : rgb_) {
            rgb.r->set_value(red);
            rgb.g->set_value(green);
            rgb.b->set_value(blue);
            rgb.w->set_value(white);
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
