#pragma once

#include "node.hh"

namespace effects {
class Fade : public runner::Node {
public:
    explicit Fade(float duration, float min = 0.0f, float max = 1.0f);

    ~Fade() = default;

    size_t input_count() const override { return 1; }
    size_t output_count() const override { return 1; }

    void callback(runner::Context& context) override;

private:
    float duration_ = 0.0f;
    float min_ = 0.0f;
    float max_ = 1.0f;
};
}