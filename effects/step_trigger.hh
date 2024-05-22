#pragma once

template <typename Effect>
class StepTrigger final : public CompositeEffect<Effect> {
public:
    ~StepTrigger() override = default;

public:
    String type() const override { return "StepTrigger(" + subtype() + ")"; }

    void trigger(uint32_t now_ms) override { 
        effect(index_)->clear(now_ms);
        index_ = (index_ + 1) % size();
        effect(index_)->trigger(now_ms);
    }

    void clear(uint32_t now_ms) override {
        effect(index_)->clear(now_ms);
    }

private:
    size_t index_ = 0;
};