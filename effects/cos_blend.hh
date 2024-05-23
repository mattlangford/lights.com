#pragma once

struct CosBlendConfig {
    std::vector<float> freq;
    std::vector<float> phase0;
};

class CosBlend final : public SingleChannelEffect {
public:
    ~CosBlend() override = default;

protected:
    float level(uint32_t now_ms) override {
        if (!enabled_) return 0.0;
        const float now_s = float(now_ms) / 1000.0;

        float value = 0.0;
        for (size_t i = 0; i < config_.freq.size(); ++i) {
            value += generate(config_.phase0[i] + config_.freq[i] * now_s, 0.0, 1.0);
        }

        return value / config_.freq.size();
    }

public:
    String type() const override { return "CosBlend"; }

    SetConfigResult set_config_json(const JsonObject& json) override {
        SetConfigResult result = SingleChannelEffect::set_config_json(json);

        auto freq = json["freq"];
        auto phase = json["phase0"];

        if (freq.size() != phase.size()) {
            result.consider(SetConfigResult::error("freq.size() != phase0.size()"));
            return result;
        }

        if (freq.size() == 0) {
            return result;
        }

        result.consider(SetConfigResult::okay());
        CosBlendConfig config;
        config.freq.resize(json["freq"].size());
        config.phase0.resize(json["phase0"].size());
        for (size_t i = 0; i < config.freq.size(); ++i) {
            config.freq[i] = json["freq"][i];
        }
        for (size_t i = 0; i < config.phase0.size(); ++i) {
            config.phase0[i] = json["phase0"][i];
        }

        set_config(config);
        return result;
    }

    void get_config_json(JsonObject& json) const override {
        SingleChannelEffect::get_config_json(json);
        JsonArray freq_array = json["freq"].to<JsonArray>();
        for (auto& freq : config_.freq) {
            freq_array.add(freq);
        }

        JsonArray phase_array = json["phase0"].to<JsonArray>();
        for (auto& phase : config_.phase0) {
            phase_array.add(phase);
        }
    }

    void set_config(const CosBlendConfig& config) {
        config_ = config;

        while (config_.phase0.size() < config_.freq.size()) {
            // Start at phase=0 if not specified
            config_.phase0.push_back(0.0);
        }
    }

    void trigger(uint32_t) override { enabled_ = true; }
    void clear(uint32_t) override { enabled_ = false; }

private:
    static float generate(float phase, float min, float max) {
        return (0.5 * (cosf(phase) + 1.0)) * (max - min) + min;
    }

    bool enabled_ = false;
    CosBlendConfig config_;
    uint32_t last_time_ms_ = 0;
};

