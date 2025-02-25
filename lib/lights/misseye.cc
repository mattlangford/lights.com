#include "misseye.hh"

namespace lights {
Missyee::Missyee(uint16_t start) : Light(start, 7) {}

void Missyee::write_to_buffer(std::vector<uint8_t>& buffer) {
    uint8_t ch = start();
    buffer[ch] = brightness_;
    buffer[ch + 1] = red_;
    buffer[ch + 2] = green_;
    buffer[ch + 3] = blue_;
}

void Missyee::callback(runner::Context& context) {
    brightness_ = cast(context.input(0));
    red_ = cast(context.input(1));
    green_ = cast(context.input(2));
    blue_ = cast(context.input(3));
}

}