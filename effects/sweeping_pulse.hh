#pragma once

#include "linear_pulse.hh"

class SweepingPulse final : public NestedEffect<CompositeEffect<LinearPulse>> {
public:
    ~SweepingPulse() override = default;

    void set_parent_config_json(const JsonObject& json)  {}

    void get_parent_config_json(JsonObject& json) const {}

    String parent_type() const { return "SweepingPulse"; }

    LinearPulse& add(uint32_t gap_ms=1000) {
        gaps_ms_.push_back(gap_ms);
        return effect().add();
    }

    void trigger(uint32_t now_ms) {
        CompositeEffect<LinearPulse>& effects = effect();
        size_t count = effects.size();

        uint64_t trigger_time = now_ms;
        for (size_t i = 0; i < count; ++i) {
            trigger_time += gaps_ms_[i];
            effects.effect(i)->trigger(trigger_time);
        }
    }

    bool is_done(uint32_t now_ms) const {
        const CompositeEffect<LinearPulse>& effects = effect();
        size_t count = effects.size();
        for (size_t i = 0; i < count; ++i) {
            if (!effects.effect(i)->is_done(now_ms)) {
                return false;
            }
        }
        return true;
    }

private:
    std::vector<uint32_t> gaps_ms_;
};

