#include "shimmer.hh"
#include <random>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <random>

namespace effects {
Shimmer::Shimmer(float frequency_min, float frequency_max, uint8_t depth, float min, float max, size_t seed) : min_(min), max_(max) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float> phase_dist(0.f, M_2_PI);
    std::uniform_real_distribution<float> freq_dist(frequency_min, frequency_max);
    for (uint8_t i = 0; i < depth; ++i) {
        params_.emplace_back(phase_dist(rng), freq_dist(rng));
    }
}

void Shimmer::callback(runner::Context& context) {
    const float dt = runner::to_seconds(context.dt());

    float result = 0.0;
    for (Params& params : params_) {
        result += std::cos(params.phase);
        params.phase = std::fmod(params.phase + dt * params.freq, M_2_PI);
    }

    // Normalize from [-N, N] to [0, 1]
    result = (result + params_.size()) / (2 * params_.size());

    // Scale to MIN to MAX
    context.output(0, result * (max_ - min_) + min_);
}
}