#pragma once

#include "linear_pulse.hh"

class SweepingPulse final : public NestedEffect<CompositeEffect<LinearPulse>> {
public:
    ~SweepingPulse() override = default;

    void set_parent_config_json(const JsonObject& json)  {
        maybe_set(json, "gap_ms", gap_ms_);
        maybe_set(json, "reversed", reversed_);
    }

    void get_parent_config_json(JsonObject& json) const {
        json["gap_ms"] = gap_ms_;
        json["reversed"] = reversed_;
    }

    String parent_type() const { return "SweepingPulse"; }

    LinearPulse& add() { return effect().add(); }

    void trigger(uint32_t now_ms) {
        CompositeEffect<LinearPulse>& effects = effect();
        size_t count = effects.size();
        for (size_t i = 0; i < count; ++i) {
            uint64_t trigger_time = now_ms;

            if (reversed_) {
                // Go from the end if we're reversed
                trigger_time += (count - 1 - i) * gap_ms_;
            } else {
                trigger_time += i * gap_ms_;
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

