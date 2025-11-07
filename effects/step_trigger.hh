#pragma once

template <typename Effect>
class StepTrigger final : public CompositeEffect<Effect> {
public:
    ~StepTrigger() override = default;

public:
    String type() const override { return "StepTrigger(" + this->subtype() + ")"; }

    void trigger(uint32_t now_ms) override { 
        if (!enabled_) {
            return;
        }
        this->effect(index_)->clear(now_ms);
        index_ = (index_ + 1) % this->size();
        this->effect(index_)->trigger(now_ms);
    }

    void clear(uint32_t now_ms) override {
        this->effect(index_)->clear(now_ms);
    }

    void set_enabled(bool enabled) { enabled_ = enabled; }

private:
    bool enabled_ = true;
    size_t index_ = 0;
};