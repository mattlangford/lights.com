#pragma once

#include "node.hh"
#include "time.hh"
#include <optional>
#include <iostream>
#include <array>

// Thoughts
//  * Enable_if constructor for 1 input
//  * one dijector class

namespace runner {
class Constant final : public Node {
public:
    Constant(float value) : Constant(std::vector{value}) {}
    Constant(std::vector<float> values) : values_(std::move(values)) { }
    size_t output_count() const override { return values_.size(); }
    void callback(Context& context) override {
        for (uint8_t i = 0; i < values_.size(); ++i) {
            context.output(i, values_[i]);
        }
    }

    void set(uint8_t index, float value) { values_.at(index) = value; }

    template <uint8_t I>
    void set(float value) {
        values_.at(I) = value;
    }

private:
    std::vector<float> values_;
};
}