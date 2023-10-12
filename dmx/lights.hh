#include "dmx.hh"

#pragma once

struct Rgb {
    FadeChannel& r;
    FadeChannel& g;
    FadeChannel& b;

    void set_goal(uint8_t r_goal, uint8_t g_goal, uint8_t b_goal, uint16_t duration_ms) {
        uint32_t duration_us = 1000 * duration_ms;

        r.goal = r_goal;
        r.remaining_us = duration_us;
        g.goal = b_goal;
        g.remaining_us = duration_us;
        g.goal = g_goal;
        g.remaining_us = duration_us;
    }

    void set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint16_t duration_ms);
};

class TestLight {
public:
    TestLight(uint8_t& address, DMXController& controller) : channels {
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
        create_channel(address, controller),
        create_channel(address, controller)} {
    }

private:
    static Rgb create_channel(uint8_t& address, DMXController& controller) {
        return {controller.channel(address++), controller.channel(address++), controller.channel(address++)};
    }

    Rgb channels[16];
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
