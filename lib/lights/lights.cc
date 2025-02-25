#include "lights.hh"

namespace lights {

namespace {
std::vector<uint16_t> from_base(uint16_t base, uint16_t count) {
    std::vector<uint8_t> output(count);
    for (uint16_t i = 0; i < count; ++i) {
        output[i] = i + base;
    }
    return output;
}
}

std::vector<uint16_t> misseye(uint16_t base) {
    // Brightness, Red, Green, Blue
    return from_base(base, 4);
}

std::vector<uint16_t> litake(uint16_t base) {
    // Brightness, Red, Green, Blue
    return from_base(base, 4);
}

std::vector<uint16_t> betopper_mover9(uint16_t base) {
    // x, y, brightness, red, green, blue, white, speed
    return from_base(base, 7);
}
std::vector<uint16_t> washbar112(uint16_t base) {
    // There are 28 lights, each has 4 channels: r, g, b, w
    return from_base(base, 28 * 4);
}
}