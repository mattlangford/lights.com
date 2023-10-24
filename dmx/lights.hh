#pragma once

#include "dmx.hh"
#include "util.hh"

class LinearFade : public Effect {
public:
    struct Config {
        uint32_t trigger_dt_ms = 1000;
        uint32_t clear_dt_ms = 1000;
        uint8_t trigger_value = 255;
        uint8_t clear_value = 0;
    };

    explicit LinearFade(Config config) { set_config(config); }
    explicit LinearFade() : LinearFade(Config{}) {}
    ~LinearFade() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        if (now_ms < start_ms_) { return clip(value + start_value_); }
        if (now_ms >= end_ms_) { return clip(value + end_value_); }

        const float fade_ratio = static_cast<float>(now_ms) / (end_ms_ - start_ms_);
        const float diff = static_cast<float>(start_value_) - end_value_;
        return clip(value + start_value_ + fade_ratio * diff);
    }

    void trigger(uint32_t now_ms) override {
        set(now_ms, config_.trigger_dt_ms, config_.trigger_value, config_.clear_value);
    }
    void clear(uint32_t now_ms) override {
        set(now_ms, config_.clear_dt_ms, config_.clear_value, config_.trigger_value);
    }

    void set_config(const Config& config) { config_ = config; }
    const Config& config() const { return config_; }

private:
    void set(uint32_t now_ms, uint32_t duration_ms, uint8_t start, uint8_t end) {
        start_ms_ = now_ms;
        end_ms_ = now_ms + duration_ms;
        start_value_ = start;
        end_value_ = end;
    }

    Config config_;

    uint32_t start_ms_ = 0;
    uint32_t end_ms_ = 0;
    uint8_t start_value_ = 0;
    uint8_t end_value_ = 0;
};

class LinearPulse : public Effect {
public:
    struct Config {
        uint32_t on_dt_ms = 1000;
        uint32_t hold_dt_ms = 1000;
        uint32_t off_dt_ms = 1000;

        uint32_t clear_dt_ms = 100;

        uint8_t trigger_value = 255;
        uint8_t clear_value = 0;
    };

    explicit LinearPulse(Config config) { set_config(config); }
    explicit LinearPulse() : LinearPulse(Config{}) {}
    ~LinearPulse() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        return (now_ms < off_start_ms_ ? on_fade_ : off_fade_).process(value, now_ms);
    }

    void trigger(uint32_t now_ms) override {
        on_fade_.trigger(now_ms);

        off_start_ms_ = now_ms + config_.on_dt_ms + config_.hold_dt_ms;
        off_fade_.trigger(off_start_ms_);
    }
    void clear(uint32_t now_ms) override {
        off_start_ms_ = now_ms;
        off_fade_.clear(now_ms);
    }

    void set_config(const Config& config) {
        config_ = config;

        uint8_t on_value = config_.trigger_value;
        uint8_t off_value = config_.clear_value;

        on_fade_.set_config({
            .trigger_dt_ms = config_.on_dt_ms,
            .clear_dt_ms = config_.clear_dt_ms,
            .trigger_value = on_value,
            .clear_value = off_value,
        });
        off_fade_.set_config({
            .trigger_dt_ms = config_.off_dt_ms,
            .clear_dt_ms = config_.clear_dt_ms,
            .trigger_value = off_value,
            .clear_value = on_value,
        });
    }
    const Config& config() const { return config_; }

private:
    Config config_;
    LinearFade on_fade_;
    LinearFade off_fade_;

    uint32_t off_start_ms_;
};

class CosBlend : public Effect {
public:
    struct Config {
        uint8_t depth = 1;
        float min_freq = 10;
        float max_freq = 1000;
        uint8_t min_value = 0;
        uint8_t max_value = 255;
    };

    explicit CosBlend(Config config) { set_config(config); }
    explicit CosBlend() : CosBlend(Config{}) {}
    ~CosBlend() override { if (waves_) { delete waves_; } }

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        const float dt = static_cast<float>(now_ms - last_time_ms_) / 1000;
        last_time_ms_ = now_ms;

        float new_value = value;
        for (size_t i = 0; i < config_.depth; ++i) {
            Wave& wave = waves_[i];
            wave.phase = fmod(wave.phase + wave.freq * dt, 2.0f * M_PI);
            value += generate(wave.phase, config_.min_value, config_.max_value);
        }

        return clip(new_value);
    }

    void trigger(uint32_t now_ms) override {}
    void clear(uint32_t now_ms) override {};

    void set_config(const Config& config) {
        config_ = config;
        waves_ = move(waves_, config_.depth);

        for (size_t i = 0; i < config_.depth; ++i) {
            waves_[i].freq = random(config_.min_freq, config_.max_freq);
        }
    }
    const Config& config() const { return config_; }

private:
    static float generate(float phase, float min, float max) {
        return (0.5 * (cosf(phase) + 1)) * (max - min) + min;
    }

    Config config_;

    struct Wave {
        float phase = 0.0;
        float freq = 0.0;
    };
    Wave* waves_ = nullptr;

    uint32_t last_time_ms_ = 0;
};

/*
template <typename Channel>
class RgbChannel {
public:
    virtual ~RgbChannel() = default;

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

template <typename Channel>
class RgbwChannel : public RgbChannel {
public:
    ~RgbwChannel() override = default;

    void set_white_goal(uint8_t w, uint32_t duration_ms=0) {
        uint32_t now_ms = millis();
        if (w_) { w_->set_goal(w, duration_ms, now_ms); }
    }

    void set_white(Channel* r) { w_ = w; }

private:
    Channel* w_;
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

*/
