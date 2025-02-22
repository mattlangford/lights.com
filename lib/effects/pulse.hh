#pragma once

#include "node.hh"

namespace effects {
class Pulse : public runner::Node {
public:
    explicit Pulse(
        float duration_on,
        float duration_off,
        float min = 0.0f,
        float max = 1.0f);

    ~Pulse() = default;

    size_t input_count() const override { return 1; }
    size_t output_count() const override { return 1; }

    void callback(runner::Context& context) override;

private:
    float duration_on_ = 0.0f;
    float duration_off_ = 0.0f;
    float min_ = 0.0f;
    float max_ = 1.0f;
};
}