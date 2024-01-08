#pragma once

struct LinearPulseConfig {
    uint32_t rise_dt_ms = 100;
    uint32_t hold_dt_ms = 1000;
    uint32_t fall_dt_ms = 2000;
    uint32_t clear_dt_ms = 100;
};

class LinearPulse final : public FaderEffect {
public:
    ~LinearPulse() override = default;

public:
    void set_config(const LinearPulseConfig& config) {
        config_ = config;
    }

    void trigger(uint32_t now_ms) override {
        uint32_t start = now_ms;
        uint32_t end = start + config_.rise_dt_ms;
        fade_to(1.0, start, end);

        if (config_.hold_dt_ms > 0) {
            start = end;
            end = start + config_.hold_dt_ms;
            fade_to(1.0, start, end);
        }

        start = end;
        end = start + config_.fall_dt_ms;
        fade_to(0.0, start, end);
    }

    void clear(uint32_t now_ms) override {
        FaderEffect::clear(now_ms);
        fade_to(0.0, now_ms, now_ms + config_.clear_dt_ms);
    }

public:
    String type() const override { return "LinearPulse"; }

    void set_config_json(const JsonObject& json) override {
        SingleChannelEffect::set_config_json(json);
        maybe_set(json, "rise_dt_ms", config_.rise_dt_ms);
        maybe_set(json, "hold_dt_ms", config_.hold_dt_ms);
        maybe_set(json, "fall_dt_ms", config_.fall_dt_ms);
        maybe_set(json, "clear_dt_ms", config_.clear_dt_ms);
    }

    void get_config_json(JsonObject& json) const override {
        SingleChannelEffect::get_config_json(json);
        json["rise_dt_ms"] = config_.rise_dt_ms;
        json["hold_dt_ms"] = config_.hold_dt_ms;
        json["fall_dt_ms"] = config_.fall_dt_ms;
        json["clear_dt_ms"] = config_.clear_dt_ms;
    }

private:
    LinearPulseConfig config_;
};

