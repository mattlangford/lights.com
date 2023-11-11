#pragma once

struct LinearFadeConfig {
    uint32_t trigger_dt_ms = 1000;
    uint32_t clear_dt_ms = 1000;
};

class LinearFade final : public ChannelEffect, public EffectBase {
public:
    ~LinearFade() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        if (now_ms < start_ms_) { return clip(value + start_value_); }
        if (now_ms >= end_ms_) { return clip(value + end_value_); }

        const float fade_ratio = (static_cast<float>(now_ms) - start_ms_) / (end_ms_ - start_ms_);
        const float diff = static_cast<float>(end_value_) - start_value_;
        return clip(value + start_value_ + fade_ratio * diff);
    }

    void trigger(uint32_t now_ms) override {
        set(now_ms, config_.trigger_dt_ms, min_, max_);
    }
    void clear(uint32_t now_ms) override {
        set(now_ms, config_.clear_dt_ms, max_, min_);
    }

    void set_config(const LinearFadeConfig& config) { config_ = config; }
    void set_values(uint8_t min, uint8_t max) { min_ = min; max_ = max; }

public:
    std::string type() const override { return "LinearFade"; }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "trigger_dt_ms", config_.trigger_dt_ms);
        maybe_set(json, "clear_dt_ms", config_.clear_dt_ms);
    }

    void get_config_json(JsonObject& json) const override {
        json["trigger_dt_ms"] = config_.trigger_dt_ms;
        json["clear_dt_ms"] = config_.clear_dt_ms;
    }

    void set_values_json(const JsonObject& json) override {
        maybe_set(json, "min", config_.trigger_dt_ms);
        maybe_set(json, "max", config_.clear_dt_ms);
    }

    void get_values_json(JsonObject& json) const override {
        json["min"] = min_;
        json["max"] = max_;
    }

private:
    void set(uint32_t now_ms, uint32_t duration_ms, uint8_t start, uint8_t end) {
        start_ms_ = now_ms;
        end_ms_ = now_ms + duration_ms;
        start_value_ = start;
        end_value_ = end;
    }

    LinearFadeConfig config_;
    uint8_t min_ = 0;
    uint8_t max_ = 255;

    uint32_t start_ms_ = 0;
    uint32_t end_ms_ = 0;
    uint8_t start_value_ = 0;
    uint8_t end_value_ = 0;
};

