#pragma once

#include "node.hh"
#include <random>

namespace effects {
class Sequencer : public runner::Node {
public:
    enum class Mode {
        STEP,
        RANDOM,
    };

public:
    explicit Sequencer(size_t count, Mode mode, size_t seed=42);

    ~Sequencer() override = default;

    size_t input_count() const override { return count_ + 1; }
    size_t output_count() const override { return 1; }

    void callback(runner::Context& context) override;

private:
    void next();

private:
    const Mode mode_ = Mode::STEP;
    const size_t count_ = 0.0f;
    size_t index_ = 0;

    std::mt19937_64 rng_;
};
}