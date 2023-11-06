#pragma once

struct LinearPulseConfig {
    uint32_t rise_dt_ms = 1000;
    uint32_t hold_dt_ms = 250;
    uint32_t fall_dt_ms = 1000;
};

class LinearPulse final : public ChannelEffect, public EffectBase {
public:
    ~LinearPulse() override = default;

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        if (now_ms < start_time_ || now_ms >= end_time_) {
            return clip(value + min_);
        }

        if (now_ms >= start_hold_time_ && now_ms < end_hold_time_) {
            return clip(value + max_);
        }

        bool up = now_ms < start_hold_time_;
        const uint32_t& start_ms = up ? start_time_ : end_hold_time_;
        const uint32_t& start_value = up ? min_ : max_;
        const uint32_t& duration = up ? config_.rise_dt_ms : config_.fall_dt_ms;
        const float diff = (up ? 1.0 : -1.0) * (max_ - min_);
        const float fade_ratio = (static_cast<float>(now_ms) - start_ms) / duration;
        return clip(value + start_value + fade_ratio * diff);
    }

    void set_config(const LinearPulseConfig& config) {
        config_ = config;
        update_times(start_time_);
    }
    void set_values(uint8_t min, uint8_t max) {
        min_ = static_cast<float>(min);
        max_ = static_cast<float>(max);
    }

    void trigger(uint32_t now_ms) override {
        update_times(now_ms);
    }
    void clear(uint32_t now_ms) override {
        end_hold_time_ = now_ms;
        end_time_ = end_hold_time_ + config_.fall_dt_ms;
    }

public:
    std::string type() const override { return "LinearPulse"; }

    void set_config_json(const JsonObject& json) override {
        bool updated = false;
        updated |= maybe_set(json, "rise_dt_ms", config_.rise_dt_ms);
        updated |= maybe_set(json, "hold_dt_ms", config_.hold_dt_ms);
        updated |= maybe_set(json, "fall_dt_ms", config_.fall_dt_ms);

        if (updated) {
            update_times(start_time_);
        }
    }

    void get_config_json(JsonObject& json) const override {
        json["rise_dt_ms"] = config_.rise_dt_ms;
        json["hold_dt_ms"] = config_.hold_dt_ms;
        json["fall_dt_ms"] = config_.fall_dt_ms;
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
    void update_times(uint32_t start_time) {
        if (start_time == 0) return;

        start_time_ = start_time;
        start_hold_time_ = start_time + config_.rise_dt_ms;
        end_hold_time_ = start_hold_time_ + config_.hold_dt_ms;
        end_time_ = end_hold_time_ + config_.fall_dt_ms;
    }

    uint32_t start_time_ = 0;
    uint32_t start_hold_time_ = 0;
    uint32_t end_hold_time_ = 0;
    uint32_t end_time_ = 0;

    LinearPulseConfig config_;
    uint8_t min_ = 0.0;
    uint8_t max_ = 255.0;
};

