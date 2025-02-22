
#include "sequencer.hh"
#include "node.hh"

#include <iostream>

namespace effects {

Sequencer::Sequencer(size_t count, Mode mode, size_t seed) : count_(count), mode_(mode), rng_(seed) { }

void Sequencer::callback(runner::Context& context) {
    // Trigger the next signal if we're above this threshold
    if (context.input(0) > 0.5) {
        next();
    }
    context.output(0, context.input(index_ + 1));
}

void Sequencer::next() {
    switch (mode_) {
    case Mode::STEP:
        index_++;
        break;
    case Mode::RANDOM: {
        std::uniform_int_distribution dist;
        index_ = dist(rng_);
        break;
    }
    }
    index_ = index_ % count_;
}
}