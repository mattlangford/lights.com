#include "math.hh"
#include "log.hh"
#include <iostream>

namespace runner {
namespace {
template <typename F>
void callback_impl(Context& context, const config::MathConfig& config, F&& f) {
    float sum = context.input(0);
    for (uint8_t i = 1; i < config.inputs; ++i) {
        sum = f(sum, context.input(i));
    }
    if (config.saturating) {
        sum = std::clamp(sum, 0.f, 1.f);
    }
    context.output(0, sum);
}
}

Adder::Adder(const config::AdderNode& config) : config_(config) { }
void Adder::callback(Context& context) {
    return callback_impl(context, config_, [](float lhs, float rhs){ return lhs + rhs; });
}

Subtractor::Subtractor(const config::SubtractorNode& config) : config_(config) { }
void Subtractor::callback(Context& context) {
    return callback_impl(context, config_, [](float lhs, float rhs){ return lhs - rhs; });
}

Multiplier::Multiplier(const config::MultiplierNode& config) : config_(config) { }
void Multiplier::callback(Context& context) {
    return callback_impl(context, config_, [](float lhs, float rhs){ return lhs * rhs; });
}

Divider::Divider(const config::DividerNode& config) : config_(config) { }
void Divider::callback(Context& context) {
    return callback_impl(context, config_, [](float lhs, float rhs){ return lhs / rhs; });
}

}