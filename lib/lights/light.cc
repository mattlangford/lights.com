#include "light.hh"

namespace light {

std::vector<uint8_t> Universe::build_buffer() const {
    std::vector<uint8_t> buffer(max_channel_, 0);
    for (const auto& light : lights_) {
        light->write_to_buffer(buffer);
    }
    return buffer;
}

void Universe::set_max_channel(uint16_t channel) {
    max_channel_ = channel;
}

Light::Light(uint16_t start, uint16_t channels) : start_(start), channels_(channels) { }

uint8_t Light::cast(float value) const {
    return static_cast<uint8_t>(std::clamp(value, 0.f, 1.f) * 255.f);
}

}