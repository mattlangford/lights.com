#pragma once

namespace runner {
enum class PortType {
    CONTROL = 0,
    GATE = 1,
    CATEGORY = 2,
};

struct ControlPort {
    using Type = PortType::CONTROL;
    using Storage = uint8_t; // 0-255 value of the signal
    std::string_view name;
};

struct GatePort {
    using Type = PortType::GATE;
    using Storage = uint16_t; // timestamp (ms) of the trigger
    std::string_view name;
};

template <uint8_t Categories>
struct CategoryPort {
    using Type = PortType::CATEGORY;
    using Storage = uint8_t; // selected option

    std::string_view name;
    std::array<std::string_view, Categories> options;
};
}