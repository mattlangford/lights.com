#pragma once

#include "dmx.hh"
#include "util.hh"

class LinearFade : public Effect {
public:
    struct Config {
        uint32_t trigger_dt_ms = 1000;
        uint32_t clear_dt_ms = 1000;
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
        set(now_ms, config_.trigger_dt_ms, min_value(), max_value());
    }
    void clear(uint32_t now_ms) override {
        set(now_ms, config_.clear_dt_ms, max_value(), min_value());
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
    };

    explicit LinearPulse(Config config) { set_config(config); }
    explicit LinearPulse() : LinearPulse(Config{}) {}
    ~LinearPulse() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        return (now_ms <= off_start_ms_ ? on_fade_ : off_fade_).process(value, now_ms);
    }

    void trigger(uint32_t now_ms) override {
        set_values();
        on_fade_.trigger(now_ms);

        off_start_ms_ = now_ms + config_.on_dt_ms + config_.hold_dt_ms;
        off_fade_.trigger(off_start_ms_);
    }
    void clear(uint32_t now_ms) override {
        set_values();
        off_start_ms_ = now_ms;
        off_fade_.clear(now_ms);
    }

    void set_config(const Config& config) {
        config_ = config;

        on_fade_.set_config({
            .trigger_dt_ms = config_.on_dt_ms,
            .clear_dt_ms = config_.clear_dt_ms,
        });
        off_fade_.set_config({
            .trigger_dt_ms = config_.off_dt_ms,
            .clear_dt_ms = config_.clear_dt_ms,
        });
    }
    const Config& config() const { return config_; }

private:
    void set_values() {
        on_fade_.set_values({.min = min_value(), .max = max_value()});
        off_fade_.set_values({.min = max_value(), .max = min_value()});
    }

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
    };

    explicit CosBlend(Config config) { set_config(config); }
    explicit CosBlend() : CosBlend(Config{}) {}
    ~CosBlend() override { if (waves_) { delete waves_; } }

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        const float dt = static_cast<float>(now_ms - last_time_ms_) / 1000;
        last_time_ms_ = now_ms;

        float new_value = 0.0;
        for (size_t i = 0; i < config_.depth; ++i) {
            Wave& wave = waves_[i];
            wave.phase = fmod(wave.phase + wave.freq * dt, 2.0f * M_PI);

            float gen = generate(wave.phase, min_value(), max_value()) / config_.depth;
            new_value += gen;
        }

        return clip(new_value);
    }

    void set_config(const Config& config) {
        config_ = config;
        waves_ = move(waves_, config_.depth);

        for (size_t i = 0; i < config_.depth; ++i) {
            waves_[i].freq = random(config_.min_freq, config_.max_freq);
            waves_[i].phase = random(0, 2 * M_PI);
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

void hsv_to_rgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);

class RgbEffect {
public:
    struct HsvValueConfig {
        float min_h;
        float max_h;

        static HsvValueConfig fixed(float v) { return HsvValueConfig{.min_h=v, .max_h=v}; }
    };

    template <typename Effect>
    static RgbEffect create() {
        return BaseRgbEffect(new Effect(), new Effect(), new Effect());
    }

    RgbEffect(Effect* r, Effect* g, Effect* b) : r_(r), g_(g), b_(b) { }

public:
    void trigger(uint32_t now_ms) {
        if (r_) { r_->trigger(now_ms); }
        if (g_) { g_->trigger(now_ms); }
        if (b_) { b_->trigger(now_ms); }
    };
    void clear(uint32_t now_ms) {
        if (r_) { r_->clear(now_ms); }
        if (g_) { g_->clear(now_ms); }
        if (b_) { b_->clear(now_ms); }
    };

    void set_values_rgb(const ValueConfig& r, const ValueConfig& g, const ValueConfig& b) {
        if (r_) { r_->set_values(r); }
        if (g_) { g_->set_values(g); }
        if (b_) { b_->set_values(b); }
    };

    // void set_values_hsv(const HsvValueConfig& h, const HsvValueConfig& s, const HsvValueConfig& v) {
    //     uint8_t r_min, g_min, b_min;
    //     uint8_t r_max, g_max, b_max;
    // };

    template <typename Effect>
    void set_config(const typename Effect::Config& config) {
        if (auto* r = dynamic_cast<Effect*>(r_)) { r->set_config(config); }
        if (auto* g = dynamic_cast<Effect*>(g_)) { g->set_config(config); }
        if (auto* b = dynamic_cast<Effect*>(b_)) { b->set_config(config); }
    }

    template <typename Effect=Effect>
    Effect* red() { return dynamic_cast<Effect*>(r_); }
    Effect* green() { return dynamic_cast<Effect*>(g_); }
    Effect* blue() { return dynamic_cast<Effect*>(b_); }

private:
    Effect* r_;
    Effect* g_;
    Effect* b_;
};

// Copied from ChatGPT
void hsv_to_rgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    // Convert HSV values to the range [0, 1]
    float h_normalized = h / 255.0;
    float s_normalized = s / 255.0;

    int i = static_cast<int>(h_normalized * 6);
    float f = (h_normalized * 6) - i;

    // Each of these will be between 0 and 255
    float p = static_cast<uint8_t>(v * (1 - s_normalized));
    float q = static_cast<uint8_t>(v * (1 - f * s_normalized));
    float t = static_cast<uint8_t>(v * (1 - (1 - f) * s_normalized));

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = 0; g = 0; b = 0;
    }
}
