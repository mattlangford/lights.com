#include "dmx.hh"

#pragma once

//
// Light primitives
//

class LinearFadeChannel : public Channel {
public:
    uint8_t value(uint32_t now_ms) const {
        if (now_ms < start_ms_ || now_ms >= end_ms_) {
            return end_value_;
        }

        const float fade_ratio = static_cast<float>(now_ms) / (end_ms_ - start_ms_);
        const float value = start_value_ + fade_ratio * (static_cast<float>(end_value_) - start_value_);
        return value < 0 ? 0 : value > 255 ? 255 : static_cast<uint8_t>(value);
    }

    bool active(uint32_t now_ms) const override { return now_ms >= start_ms_ && now_ms < end_ms_; }

public:
    void set_goal(uint8_t new_goal, uint32_t duration_ms=0) { set_goal(new_goal, duration_ms, now()); }
    void set_goal(uint8_t new_goal, uint32_t duration_ms, uint32_t now_ms) {
        start_ms_ = now_ms;
        end_ms_ = now_ms + duration_ms;
        start_value_ = current_value();
        end_value_ = new_goal;
    }

private:
    uint32_t start_ms_ = 0;
    uint32_t end_ms_ = 0;
    uint8_t start_value_ = 0;
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

    uint8_t value(uint32_t now_ms, uint8_t last_value) const {
        uint16_t sum = 0;
        for (uint8_t i = 0; i < count_; ++i) {
            sum += layers_[i]->get_value(now_ms);
        }

        // Clip the value, and inform each channel about the current total value
        uint8_t value = sum > 255 ? 255 : static_cast<uint8_t>(sum);
        for (uint8_t i = 0; i < count_; ++i) {
            layers_[i]->set_value(value);
        }
        return value;
    }

    bool active(uint32_t now_ms) const override {
        for (uint8_t i = 0; i < count_; ++i) {
            if (layers_[i]->active(now_ms)) {
                return true;
            }
        }
        return false;
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
        if (r_) { r_->set_goal(r, duration_ms, now_ms); }
        if (g_) { g_->set_goal(g, duration_ms, now_ms); }
        if (b_) { b_->set_goal(b, duration_ms, now_ms); }
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

    LinearFadeChannel& brightness() {
        return channels[0];
    }

    LinearFadeChannel channels[3 * NUM_LIGHTS + 1];
    LinearFadeChannel bonus_channels[3];
    RgbChannel<LinearFadeChannel> rgb[NUM_LIGHTS];
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
