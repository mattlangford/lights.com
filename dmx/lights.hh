#include "dmx.hh"

#pragma once

//
// Light primitives
//

class SimpleChannel : public Channel {
public:
    ~SimpleChannel() override = default;

    void set_goal(uint8_t new_goal, uint32_t duration_ms=0) {
        set_goal_at(new_goal, duration_ms, now());
    }
    void set_goal_at(uint8_t new_goal, uint32_t duration_ms, uint32_t now_ms) {
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

class LayeredChannel : public Channel {
public:
    ~LayeredChannel() override = default;

public:
    void add_layer(ChannelPtr channel) {
        uint8_t last = count_;
        count_++;

        layers_ = reinterpret_cast<ChannelPtr*>(realloc(layers_, sizeof(ChannelPtr) * count_));
        layers_[last] = channel;
    }

    Channel* channel(uint8_t layer) {
        if (layer < count_) {
            return layers_[layer];
        }
        return nullptr;
    }

    uint8_t get_value_at(uint32_t now_ms) const override {
        uint16_t sum = 0;
        for (uint8_t i = 0; i < count_; ++i) {
            sum += layers_[i]->get_value_at(now_ms);
        }
        return sum > 255 ? 255 : static_cast<uint8_t>(sum);
    }
    bool done_at(uint32_t now_ms) const override {
        for (uint8_t i = 0; i < count_; ++i) {
            if (!layers_[i]->done_at(now_ms)) {
                return false;
            }
        }
        return true;
    }

private:
    uint8_t count_ = 0;
    ChannelPtr* layers_ = nullptr;
};


template <typename Channel>
class RgbChannel {
public:
    void set_goal(uint8_t r, uint8_t g, uint8_t b, uint32_t duration_ms=0) {
        uint32_t now_ms = millis();
        if (r_) { r_->set_goal_at(r, duration_ms, now_ms); }
        if (g_) { g_->set_goal_at(g, duration_ms, now_ms); }
        if (b_) { b_->set_goal_at(b, duration_ms, now_ms); }
    }
    void set_goal_hsv(uint8_t h, uint8_t s, uint8_t v, uint32_t duration_ms=0);

    void set_red(Channel* r) { r_ = r; }
    void set_green(Channel* g) { g_ = g; }
    void set_blue(Channel* b) { b_ = b; }

private:
    Channel* r_;
    Channel* g_;
    Channel* b_;
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

    SimpleChannel& brightness() {
        return channels[0];
    }

    SimpleChannel channels[3 * NUM_LIGHTS + 1];
    SimpleChannel bonus_channels[3];
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
