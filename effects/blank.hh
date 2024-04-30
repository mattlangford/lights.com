#pragma once

#include <algorithm>

class Blank final : public FaderEffect {
private:
    static constexpr uint32_t FADE_TIME_MS = 500;
public:
    Blank() {
        // Default to blocking all incoming signals.
        set_input_gain(0.0);
    }
    ~Blank() override {}

public:
    // Only use the level function to set the input gain.
    float level(uint32_t now_ms) override {
        if (goal_ == NONE) {
            return 0.0;
        }

        set_input_gain(0.0);
        if (now_ms < start_time_) {
            return 0.0;
        }

        if (now_ms > end_time_) {
            set_input_gain(goal_ == BLANK ? 0.0 : 1.0);
            goal_ = NONE;
            return 0.0;
        }

        const float duration = static_cast<float>(end_time_) - start_time_;
        const float percent = constrain((static_cast<float>(now_ms) - start_time_) / duration, 0.0, 1.0);
        if (goal_ == BLANK) {
            set_input_gain(1.0 - percent);
        } else {
            set_input_gain(percent);
        }
        return 0.0;
    }

    void trigger(uint32_t now_ms) override {
        start_time_ = now_ms;
        end_time_ = now_ms + FADE_TIME_MS;
        goal_ = BLANK;
    }
    void clear(uint32_t now_ms) override {
        start_time_ = now_ms;
        end_time_ = now_ms + FADE_TIME_MS;
        goal_ = UNBLANK;
    }

    String type() const override { return "Blank"; }

private:
    enum Goal { BLANK, UNBLANK, NONE };
    Goal goal_ = NONE;

    uint32_t start_time_ = 0;
    uint32_t end_time_ = 0;
};


