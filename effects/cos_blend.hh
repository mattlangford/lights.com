#pragma once

struct CosBlendConfig {
    uint8_t depth = 1;
    float min_freq = 10;
    float max_freq = 1000;
};

class CosBlend final : public SingleChannelEffect {
public:
    ~CosBlend() override = default;

    CosBlend() { update_waves(); }

protected:
    float level(uint32_t now_ms) override {
        const float dt = static_cast<float>(now_ms - last_time_ms_) / 1000;
        last_time_ms_ = now_ms;

        float value = 0.0;
        for (auto& wave : waves_) {
            wave.phase = fmod(wave.phase + wave.freq * dt, 2.0f * M_PI);
            value += generate(wave.phase, min_value(), max_value());
        }
        return value / config_.depth;
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

    void set_config(const CosBlendConfig& config) {
        config_ = config;
        update_waves();
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
};

