#pragma once

#include "linear_fade.hh"

struct LinearPulseConfig {
    uint32_t rise_dt_ms = 100;
    uint32_t hold_dt_ms = 1000;
    uint32_t fall_dt_ms = 2000;
    uint32_t clear_dt_ms = 100;
};

class LinearPulse final : public ChannelEffect, public EffectBase {
public:
    ~LinearPulse() override = default;

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        return clip(value + (max_ - min_) * fader_.value_at(now_ms) + min_);
    }

    void set_config(const LinearPulseConfig& config) {
        config_ = config;
    }
    void set_values(uint8_t min, uint8_t max) {
        min_ = static_cast<float>(min);
        max_ = static_cast<float>(max);
    }

    void trigger(uint32_t now_ms) override {
        uint32_t start = now_ms;

        uint32_t end = start + config_.rise_dt_ms;
        fader_.fade_to(1.0, start, end);

        if (config_.hold_dt_ms > 0) {
            start = end;
            end = start + config_.hold_dt_ms;
            fader_.fade_to(1.0, start, end);
        }

        start = end;
        end = start + config_.fall_dt_ms;
        fader_.fade_to(0.0, start, end);
    }

    void clear(uint32_t now_ms) override {
        fader_.clear(now_ms);
        fader_.fade_to(0.0, now_ms, now_ms + config_.clear_dt_ms);
    }

public:
    std::string type() const override { return "LinearPulse"; }

    void set_config_json(const JsonObject& json) override {
        bool updated = false;
        updated |= maybe_set(json, "rise_dt_ms", config_.rise_dt_ms);
        updated |= maybe_set(json, "hold_dt_ms", config_.hold_dt_ms);
        updated |= maybe_set(json, "fall_dt_ms", config_.fall_dt_ms);
        updated |= maybe_set(json, "clear_dt_ms", config_.clear_dt_ms);
    }

    void get_config_json(JsonObject& json) const override {
        json["rise_dt_ms"] = config_.rise_dt_ms;
        json["hold_dt_ms"] = config_.hold_dt_ms;
        json["fall_dt_ms"] = config_.fall_dt_ms;
        json["clear_dt_ms"] = config_.clear_dt_ms;
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
    Fader fader_;
    LinearPulseConfig config_;
    uint8_t min_ = 0.0;
    uint8_t max_ = 255.0;
};

