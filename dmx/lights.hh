#include "dmx.hh"

#pragma once

//
// Light primitives
//

class SimpleChannel : public Channel {
public:
    ~SimpleChannel() override = default;

    void set_goal_at(uint8_t new_goal, uint32_t duration_ms, uint32_t now_ms) override {
        const uint8_t current = get_value_at(now_ms);
        slope_ = (static_cast<float>(new_goal) - static_cast<float>(current)) / static_cast<float>(duration_ms);

        end_value_ = new_goal;
        end_ms_ = now_ms + duration_ms;
    }

    uint8_t get_value_at(uint32_t now_ms) const override {
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

    bool done_at(uint32_t now_ms) const override {
        return now_ms >= end_ms_;
    }

private:
    float slope_ = 0.0;
    uint32_t end_ms_ = 0;
    uint8_t end_value_ = 0;
};

template <typename Channel>
class RgbChannel {
public:
    void set_goal(uint8_t r, uint8_t g, uint8_t b, uint32_t duration_ms=0) {
        uint32_t now_ms = millis();
        r_.set_goal_at(r, duration_ms, now_ms);
        g_.set_goal_at(g, duration_ms, now_ms);
        b_.set_goal_at(b, duration_ms, now_ms);
    }
    void set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint32_t duration_ms=0);

    Channel& red_channel() { return r_; };
    Channel& green_channel() { return g_; };
    Channel& blue_channel() { return b_; };

private:
    Channel r_;
    Channel g_;
    Channel b_;
};

//
// Light Types
//

class WashLightBar52 {
public:
    static constexpr uint8_t NUM_LIGHTS = 16;
    static constexpr uint8_t NUM_CHANNELS = 52;

    WashLightBar52(uint8_t address, DMXController& controller) {
        controller.set_max_channel(address + NUM_CHANNELS);
        controller.add_channel(address++, brightness);
        address++;
        for (uint8_t light = 0; light < NUM_LIGHTS; ++light) {
            controller.add_channel(address++, rgb[light].red_channel());
            controller.add_channel(address++, rgb[light].green_channel());
            controller.add_channel(address++, rgb[light].blue_channel());
        }
    }

    void set_goal(uint8_t r, uint8_t g, uint8_t b, uint32_t duration_ms) {
        for (uint8_t light = 0; light < NUM_LIGHTS; ++light) {
            rgb[light].set_goal(r, g, b, duration_ms);
        }
    }

    SimpleChannel brightness;
    RgbChannel<SimpleChannel> rgb[NUM_LIGHTS];
};

// Copied from ChatGPT
template <typename Channel>
void RgbChannel<Channel>::set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint32_t duration_ms) {
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
