#pragma once

#include "node.hh"
#include "time.hh"
#include <optional>
#include <array>

// Thoughts
//  * Enable_if constructor for 1 input
//  * one dijector class

namespace runner {
template <uint8_t Count=1>
class Constant final : public Node {
public:
    Constant(const std::array<float, Count>& values) : values_(values) { }
    size_t output_count() const override { return Count; }
    void callback(Context& context) override {
        for (uint8_t i = 0; i < Count; ++i) {
            context.output(i, values_[i]);
        }
    }

private:
    std::array<float, Count> values_;
};

template <uint8_t Count=1>
class Saver final : public Node {
public:
    Saver() = default;
    size_t input_count() const override { return Count; }
    void callback(Context& context) override {
        time = context.now();
        for (uint8_t i = 0; i < Count; ++i) {
            values_[i] = context.input(i);
        }
    }

    float value(uint8_t index) const { return values_[index]; }

    template <uint8_t I>
    float value() const {
        static_assert(I < Count, "Index out of bounds!");
        return values_[I];
    }

    Time time;

private:
    std::array<float, Count> values_;
};
}