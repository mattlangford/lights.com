#pragma once

#include "node.hh"

namespace effects {
class Shimmer : public runner::Node {
public:
    explicit Shimmer(
        float frequency_min,
        float frequency_max,
        uint8_t depth,
        float min = 0.0f,
        float max = 1.0f,
        size_t seed = 42);

    ~Shimmer() override = default;

    size_t output_count() const override { return 1; }

    void callback(runner::Context& context) override;

private:
    struct Params {
        float phase;
        float freq;
    };

    std::vector<Params> params_;

    float min_ = 0.0f;
    float max_ = 1.0f;
};
}