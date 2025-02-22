#include "fade.hh"
#include <algorithm>

namespace effects {
Fade::Fade(float duration, float min, float max) : duration_(duration), min_(min), max_(max) {}

void Fade::callback(runner::Context& context) {
    const float trigger_time = context.input(0);
    const float now = runner::to_seconds(context.now());

    // return 0 if trigger_time <= now and 1 if now > trigger_time + duration_. Linearly interpolate between.
    const float p = std::clamp((now - trigger_time) / duration_, 0.f, 1.f);
    const float output = p * (max_ - min_) + min_;
    context.output(0, output);
}
}