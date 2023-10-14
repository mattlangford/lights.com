#include "dmx.hh"

#pragma once

struct Rgb {
    FadeChannel& r;
    FadeChannel& g;
    FadeChannel& b;

    void set_goal(uint8_t r_goal, uint8_t g_goal, uint8_t b_goal, uint16_t duration_ms=0) {
        r.set_goal(r_goal, duration_ms);
        g.set_goal(g_goal, duration_ms);
        b.set_goal(b_goal, duration_ms);
    }

    void set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint16_t duration_ms=0);
};

class WashLightBar52 {
public:
    static constexpr uint8_t NUM_LIGHTS = 16;
    WashLightBar52(uint8_t& address, DMXController& controller) :
        brightness(controller.channel(address++)),
        rgb {
            create_channel(++address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller),
            create_channel(address, controller)} {
        address += 2;
    }

    static Rgb create_channel(uint8_t& address, DMXController& controller) {
        return {controller.channel(address++), controller.channel(address++), controller.channel(address++)};
    }

    FadeChannel& brightness;
    Rgb rgb[NUM_LIGHTS];
};

// Copied from ChatGPT
void Rgb::set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint16_t duration_ms) {
    // Convert HSV values to the range [0, 1]
    float h_normalized = static_cast<float>(h) / 255.0;
    float s_normalized = static_cast<float>(s) / 255.0;
    float v_normalized = static_cast<float>(v) / 255.0;

    int i = static_cast<int>(h_normalized * 6);
    float f = (h_normalized * 6) - i;
    float p = v_normalized * (1 - s_normalized);
    float q = v_normalized * (1 - f * s_normalized);
    float t = v_normalized * (1 - (1 - f) * s_normalized);

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    switch (i % 6) {
        case 0:
            r = static_cast<uint8_t>(v_normalized * 255);
            g = static_cast<uint8_t>(t * 255);
            b = static_cast<uint8_t>(p * 255);
            break;
        case 1:
            r = static_cast<uint8_t>(q * 255);
            g = static_cast<uint8_t>(v_normalized * 255);
            b = static_cast<uint8_t>(p * 255);
            break;
        case 2:
            r = static_cast<uint8_t>(p * 255);
            g = static_cast<uint8_t>(v_normalized * 255);
            b = static_cast<uint8_t>(t * 255);
            break;
        case 3:
            r = static_cast<uint8_t>(p * 255);
            g = static_cast<uint8_t>(q * 255);
            b = static_cast<uint8_t>(v_normalized * 255);
            break;
        case 4:
            r = static_cast<uint8_t>(t * 255);
            g = static_cast<uint8_t>(p * 255);
            b = static_cast<uint8_t>(v_normalized * 255);
            break;
        case 5:
            r = static_cast<uint8_t>(v_normalized * 255);
            g = static_cast<uint8_t>(p * 255);
            b = static_cast<uint8_t>(q * 255);
            break;
    }

    set_goal(r, g, b, duration_ms);
}
