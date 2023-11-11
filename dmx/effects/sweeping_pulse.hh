#pragma once

#include "linear_pulse.hh"

class SweepingPulse final : public EffectBase {
public:
    ~SweepingPulse() override = default;

    std::string type() const override { return "SweepingPulse"; }

    LinearPulse& add() {
        effects_.push_back(std::make_unique<LinearPulse>());
        return *effects_.back();
    }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "gap_ms", gap_ms_);
        maybe_set(json, "reversed", reversed_);
        for (auto& effect : effects_) {
            effect->set_config_json(json["pulse_config"]);
        }
    }

    void get_config_json(JsonObject& json) const {
        json["gap_ms"] = gap_ms_;
        json["reversed"] = reversed_;
        JsonObject pulse_config = json.createNestedObject("pulse_config");
        if (!effects_.empty()) {
            // Expecting these all to the same, just pick the first
            effects_.front()->get_config_json(pulse_config);
        }
    };

    void set_values_json(const JsonObject& json) {
        for (auto& effect : effects_) {
            effect->set_values_json(json);
        }
    }

    void get_values_json(JsonObject& json) const {
        if (!effects_.empty()) {
            // Expecting these all to the same, just pick the first
            effects_.front()->get_values_json(json);
        }
    };

    void trigger(uint32_t now_ms) {
        if (reversed_) {
            size_t i = 0;
            for (auto it = effects_.rbegin(); it != effects_.rend(); ++it, ++i) {
                (*it)->trigger(now_ms + i * gap_ms_);
            }
        } else {
            for (size_t i = 0; i < effects_.size(); ++i) {
                effects_[i]->trigger(now_ms + i * gap_ms_);
            }
        }
    }
    void clear(uint32_t now_ms) {
        for (auto& effect : effects_) {
            effect->clear(now_ms);
        }
    }

    void reverse() {
        reversed_ = !reversed_;
    }

private:
    uint32_t gap_ms_ = 100;
    bool reversed_ = false;
    std::vector<std::unique_ptr<LinearPulse>> effects_;
};

