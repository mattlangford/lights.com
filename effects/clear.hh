#pragma once

#include <algorithm>

class Clear final : public FaderEffect {
public:
    Clear() {
        set_value(0.0);
        set_input_gain(0.0);
    }
    ~Clear() override {}

public:
    // Only use the level function to set the input gain.
    float level(uint32_t now_ms) override {
        if (goal == NONE) {
            return;
        }

        set_input_gain(0.0);
        if (now_ms < start_time) {
            return 0.0;
        }

        if (now_ms > end_time_) {
            set_input_gain(goal_ == BLANK ? 0.0 : 1.0);
            goal_ = NONE;
            return 0.0;
        }

        const float duration = static_cast<float>(end_time_) - start_time_;
        const float percent = std::clamp((static_cast<float>(now_ms) - start_time_) / duration, 0.0, 1.0);
        if (goal_ == BLANK) {
            set_input_gain(1.0 - percent);
        } else {
            set_input_gain(percent);
        }
        return 0.0;
    }

    void trigger(uint32_t now_ms) override {
        start_time_ = now_ms;
        end_time_ = now_ms;
    }
    void clear(uint32_t now_ms) override {
        start_time_ = now_ms;
        end_time_ = now_ms;
    }

    String type() const override { return "Clear"; }

private:
    enum Goal { BLANK, UNBLANK, NONE };
    Goal goal_ = NONE;

    uint32_t start_time_ = 0;
    uint32_t end_time_ = 0;
};


