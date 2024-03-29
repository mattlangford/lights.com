#pragma once

#include <algorithm>

struct LinearFadeConfig {
    uint32_t trigger_dt_ms = 1000;
    uint32_t clear_dt_ms = 1000;
};

class LinearFade final : public FaderEffect {
public:
    ~LinearFade() override {}

public:
    void trigger(uint32_t now_ms) override {
        fade_to(1.0, now_ms, config_.trigger_dt_ms);
    }
    void clear(uint32_t now_ms) override {
        FaderEffect::clear(now_ms);
        fade_to(0.0, now_ms, now_ms + config_.clear_dt_ms);
    }

    String type() const override { return "LinearFade"; }

    SetConfigResult set_config_json(const JsonObject& json) override {
        SetConfigResult result = SingleChannelEffect::set_config_json(json);
        result.maybe_set(json, "trigger_dt_ms", config_.trigger_dt_ms);
        result.maybe_set(json, "clear_dt_ms", config_.clear_dt_ms);
        return result;
    }

    void get_config_json(JsonObject& json) const override {
        SingleChannelEffect::get_config_json(json);
        json["trigger_dt_ms"] = config_.trigger_dt_ms;
        json["clear_dt_ms"] = config_.clear_dt_ms;
    }

    void set_config(const LinearFadeConfig& config) { config_ = config; }

private:
    LinearFadeConfig config_;
};

