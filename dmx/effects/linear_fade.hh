#pragma once

#include <algorithm>

class Fader {
public:
    struct FadePoint {
        uint32_t time_ms = 0;
        float value = 0.0;
    };

public:
    void fade_to(float end, uint32_t now_ms, uint32_t end_ms) {
        if (!fades_.empty() && end_ms < fades_.back().time_ms) {
            fades_.push_back(FadePoint{now_ms, value_at(now_ms)});
        }

        fades_.push_back(FadePoint{end_ms, end});
        std::sort(fades_.begin(), fades_.end(),
                [](const FadePoint& lhs, const FadePoint& rhs){
                    return lhs.time_ms < rhs.time_ms;
                });
    }

    float value_at(uint32_t now_ms) {
        if (fades_.empty()) return 0.0;
        if (now_ms < fades_.front().time_ms) return fades_.front().value;

        for (size_t i = 0; i < fades_.size() - 1; ++i) {
            const FadePoint& next = fades_[i + 1];
            if (now_ms > next.time_ms) {
                continue;
            }
            const FadePoint& prev = fades_[i];
            const float percent = (static_cast<float>(now_ms) - prev.time_ms) / (next.time_ms - prev.time_ms);
            return prev.value + percent * (next.value - prev.value);
        }

        return fades_.back().value;
    }

    void clear(uint32_t now_ms) {
        FadePoint now{now_ms, value_at(now_ms)};
        fades_.clear();
        fades_.emplace_back(std::move(now));
    }

private:
    std::vector<FadePoint> fades_;
};


struct LinearFadeConfig {
    uint32_t trigger_dt_ms = 1000;
    uint32_t clear_dt_ms = 1000;
};

class LinearFade final : public ChannelEffect, public EffectBase {
public:
    ~LinearFade() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        return clip(value + (max_ - min_) * fader_.value_at(now_ms) + min_);
    }

    void trigger(uint32_t now_ms) override {
        fader_.fade_to(1.0, now_ms, config_.trigger_dt_ms);
    }
    void clear(uint32_t now_ms) override {
        fader_.clear(now_ms);
        fader_.fade_to(0.0, now_ms, config_.clear_dt_ms);
    }

    void set_config(const LinearFadeConfig& config) { config_ = config; }
    void set_values(uint8_t min, uint8_t max) { min_ = min; max_ = max; }

public:
    String type() const override { return "LinearFade"; }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "trigger_dt_ms", config_.trigger_dt_ms);
        maybe_set(json, "clear_dt_ms", config_.clear_dt_ms);
    }

    void get_config_json(JsonObject& json) const override {
        json["trigger_dt_ms"] = config_.trigger_dt_ms;
        json["clear_dt_ms"] = config_.clear_dt_ms;
    }

    void set_values_json(const JsonObject& json) override {
        maybe_set(json, "min", min_);
        maybe_set(json, "max", max_);
    }

    void get_values_json(JsonObject& json) const override {
        json["min"] = min_;
        json["max"] = max_;
    }

private:
    Fader fader_;
    LinearFadeConfig config_;
    uint8_t min_ = 0;
    uint8_t max_ = 255;
};

