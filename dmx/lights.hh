#include "dmx.hh"

#pragma once

//
// Light primitives
//

class SimpleChannel {
public:
    void set_goal(uint8_t new_goal, uint32_t duration_ms=0) { set_goal(new_goal, duration_ms, millis()); }
    void set_goal(uint8_t new_goal, uint32_t duration_ms, uint32_t now_ms) {
        const uint8_t current = value(now_ms);
        slope_ = (static_cast<float>(new_goal) - static_cast<float>(current)) / static_cast<float>(duration_ms);

        end_value_ = new_goal;
        end_ms_ = now_ms + duration_ms;
    }

    inline bool done() const {
        return millis() > end_ms_;
    }

    uint8_t value(uint32_t now_ms) const {
        if (now_ms >= end_ms_) {
            return end_value_;
        }

        const uint32_t remaining = end_ms_ - now_ms;
        const float value = end_value_ - slope_ * remaining;

        if (value < 0) {
            return 0;
        } else if (value > 255) {
            return 255;
        }
        return value;
    }

    ChannelCallback callback() {
        return ChannelCallback(&SimpleChannel::invoke, this);
    }

private:
    static uint8_t invoke(uint32_t now_ms, void* context) {
        return cast<SimpleChannel>(context).value(now_ms);
    }

    float slope_ = 0.0;
    uint32_t end_ms_ = 0;
    uint8_t end_value_ = 0;
};

class RgbChannel {
public:
    void set_goal(uint8_t r_goal, uint8_t g_goal, uint8_t b_goal, uint16_t duration_ms=0) {
        uint32_t now_ms = millis();
        r.set_goal(r_goal, duration_ms, now_ms);
        g.set_goal(g_goal, duration_ms, now_ms);
        b.set_goal(b_goal, duration_ms, now_ms);
    }
    void set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint16_t duration_ms=0);

    ChannelCallback red_callback() {
        return ChannelCallback(&RgbChannel::invoke, &r);
    }
    ChannelCallback green_callback() {
        return ChannelCallback(&RgbChannel::invoke, &g);
    }
    ChannelCallback blue_callback() {
        return ChannelCallback(&RgbChannel::invoke, &b);
    }

private:
    SimpleChannel r;
    SimpleChannel g;
    SimpleChannel b;

    static uint8_t invoke(uint32_t now_ms, void* context) {
        return cast<SimpleChannel>(context).value(now_ms);
    }
};

//
// Light Types
//

class WashLightBar52 {
public:
    static constexpr uint8_t NUM_LIGHTS = 16;
    static constexpr uint8_t NUM_CHANNELS = 52;

    WashLightBar52(uint8_t address, DMXController& controller) {
        controller.set_max_channel(NUM_CHANNELS);
        controller.add_callback(address++, brightness.callback());
        address++;
        for (uint8_t light = 0; light < NUM_LIGHTS; ++light) {
            controller.add_callback(address++, rgb[light].red_callback());
            controller.add_callback(address++, rgb[light].green_callback());
            controller.add_callback(address++, rgb[light].blue_callback());
        }
    }

    SimpleChannel brightness;
    RgbChannel rgb[NUM_LIGHTS];
};

// Copied from ChatGPT
void RgbChannel::set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint16_t duration_ms) {
    // Convert HSV values to the range [0, 1]
    float h_normalized = static_cast<float>(h) / 255.0;
    float s_normalized = static_cast<float>(s) / 255.0;
    float v_normalized = static_cast<float>(v) / 255.0;

    int i = static_cast<int>(h_normalized * 6);
    float f = (h_normalized * 6) - i;
    float p = v_normalized * (1 - s_normalized);
    float q = v_normalized * (1 - f * s_normalized);
    float t = v_normalized * (1 - (1 - f) * s_normalized);

    float r, g, b;
    switch (i % 6) {
        case 0: r = v_normalized; g = t; b = p; break;
        case 1: r = q; g = v_normalized; b = p; break;
        case 2: r = p; g = v_normalized; b = t; break;
        case 3: r = p; g = q; b = v_normalized; break;
        case 4: r = t; g = p; b = v_normalized; break;
        case 5: r = v_normalized; g = p; b = q; break;
        default: r = 0.0; g = 0.0; b = 0.0;
    }

    set_goal(255 * r, 255 * g, 255 * b, duration_ms);
}
