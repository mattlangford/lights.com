#include "universe.hh"
#include <algorithm>

namespace light {
Universe::Universe(const config::UniverseNode& config) {
    for (const auto& light : config.lights) {
        add_light(light);
    }
}

void Universe::add_light(config::UniverseNode::Light light) {
    for (const auto& channel : light.channels) {
        if (channel >= channels_.size()) {
            set_max_channel(channel);
        }
    }
    lights_.push_back(std::move(light));
}

void Universe::set_max_channel(uint16_t channel) {
    channels_.resize(channel + 1, 0);
}

size_t Universe::input_count() const {
    size_t count = 0;
    for (const auto& light : lights_) {
        count += light.channels.size();
    }
    return count;
}

void Universe::callback(runner::Context& context) {
    size_t index = 0;
    for (const auto& light : lights_) {
        for (const auto& channel : light.channels) {
            channels_[channel] = static_cast<uint8_t>(std::clamp(context.input(index++), 0.f, 1.f) * 255.f);
        }
    }
}

const std::vector<uint8_t>& Universe::channels() const {
    return channels_;
}
}