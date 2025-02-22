#include "pulse.hh"
#include <algorithm>

namespace effects {
Pulse::Pulse(float duration_on, float duration_off, float min, float max) :
    duration_on_(duration_on), duration_off_(duration_off), min_(min), max_(max) {}

void Pulse::callback(runner::Context& context) {
    const float trigger_time = context.input(0);
    const float trigger_off_time = trigger_time + duration_on_;
    const float now = runner::to_seconds(context.now());

    float percent = 0.0f;
    if (now > trigger_off_time) {
        percent = 1.f - std::clamp((now - trigger_off_time) / duration_off_, 0.f, 1.f);
    } else if (now > trigger_time) {
        percent = std::clamp((now - trigger_time) / duration_on_, 0.f, 1.f);
    }
    context.output(0, percent * (max_ - min_) + min_);
}
}