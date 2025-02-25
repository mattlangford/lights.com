#include "math.hh"
#include "log.hh"
#include <iostream>

namespace runner {
namespace {
template <typename F>
void callback_impl(Context& context, uint8_t inputs, F&& f) {
    float sum = context.input(0);
    for (uint8_t i = 1; i < inputs; ++i) {
        sum = f(sum, context.input(i));
    }
    context.output(0, sum);
}
}

Adder::Adder(const config::AdderNode& config) : config_(config) { }
void Adder::callback(Context& context) {
    return callback_impl(context, config_.inputs, [](float lhs, float rhs){ return lhs + rhs; });
}

Subtractor::Subtractor(const config::SubtractorNode& config) : config_(config) { }
void Subtractor::callback(Context& context) {
    return callback_impl(context, config_.inputs, [](float lhs, float rhs){ return lhs - rhs; });
}

Multiplier::Multiplier(const config::MultiplierNode& config) : config_(config) { }
void Multiplier::callback(Context& context) {
    return callback_impl(context, config_.inputs, [](float lhs, float rhs){ return lhs * rhs; });
}

Divider::Divider(const config::DividerNode& config) : config_(config) { }
void Divider::callback(Context& context) {
    return callback_impl(context, config_.inputs, [](float lhs, float rhs){ return lhs / rhs; });
}

}