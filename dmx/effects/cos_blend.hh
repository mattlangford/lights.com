#pragma once

struct CosBlendConfig {
    uint8_t depth = 1;
    float min_freq = 10;
    float max_freq = 1000;
};

class CosBlend final : public ChannelEffect, public EffectBase {
public:
    ~CosBlend() override = default;

    CosBlend() { update_waves(); }

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        const float dt = static_cast<float>(now_ms - last_time_ms_) / 1000;
        last_time_ms_ = now_ms;

        float new_value = value;
        for (auto& wave : waves_) {
            wave.phase = fmod(wave.phase + wave.freq * dt, 2.0f * M_PI);
            new_value += generate(wave.phase, min_, max_);
        }

        return clip(new_value / config_.depth);
    }

    void set_config(const CosBlendConfig& config) {
        config_ = config;
        update_waves();
    }
    void set_values(uint8_t min, uint8_t max) {
        min_ = static_cast<float>(min);
        max_ = static_cast<float>(max);
    }

public:
    String type() const override { return "CosBlend"; }

    void set_config_json(const JsonObject& json) override {
        bool updated = false;
        updated |= maybe_set(json, "depth", config_.depth);
        updated |= maybe_set(json, "min_freq", config_.min_freq);
        updated |= maybe_set(json, "max_freq", config_.max_freq);

        if (updated) {
            update_waves();
        }
    }

    void get_config_json(JsonObject& json) const override {
        json["depth"] = config_.depth;
        json["min_freq"] = config_.min_freq;
        json["max_freq"] = config_.max_freq;
    }

    void set_values_json(const JsonObject& json) override {
        uint8_t value = 0;
        if (maybe_set(json, "min", value)) {
            min_ = static_cast<float>(value);
        }
        if (maybe_set(json, "max", value)) {
            max_ = static_cast<float>(value);
        }
    }

    void get_values_json(JsonObject& json) const override {
        json["min"] = static_cast<uint8_t>(min_);
        json["max"] = static_cast<uint8_t>(max_);
    }

private:
    static float generate(float phase, float min, float max) {
        return (0.5 * (cosf(phase) + 1)) * (max - min) + min;
    }

    void update_waves() {
        waves_.resize(config_.depth);

        for (size_t i = 0; i < config_.depth; ++i) {
            waves_[i].freq = random(config_.min_freq, config_.max_freq);
            waves_[i].phase = random(0, 2 * M_PI);
        }
    }

    CosBlendConfig config_;

    struct Wave {
        float phase = 0.0;
        float freq = 0.0;
    };
    std::vector<Wave> waves_;

    uint32_t last_time_ms_ = 0;
    float min_ = 0.0;
    float max_ = 255.0;
};

