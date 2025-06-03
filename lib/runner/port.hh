#pragma once

#include "time.hh"
#include <type_traits>
#include <chrono>

namespace runner {
struct PortTag {};

template<typename T>
constexpr bool is_port_v = std::is_base_of_v<PortTag, T>;

enum class PortType {
    VALUE = 0,
    GATE = 1,
    OPTION = 2,
};

/// @brief The port represents a connection between two nodes. The producer writes to the storage, readers read from it.
template <PortType Type, typename StorageT>
struct Port : PortTag {
    static constexpr PortType type = Type;
    using Storage = StorageT;

    constexpr Port(std::string_view name) : name(name), storage(nullptr) {}

    std::string_view name;
    Storage* storage;
};

/// @brief A Value port, contains a single value
struct Value {
    float data;
    void write(float in) { data = in; }
    float read() const { return data; }
};
using ValuePort = Port<PortType::VALUE, Value>;

/// @brief A Gate port, contains the time the gate was activated.
struct Gate {
    Time trigger{std::chrono::seconds(0)};
    void write() { trigger = std::chrono::steady_clock::now(); }
    Time read() const { return trigger; }
};
using GatePort = Port<PortType::GATE, Gate>;

/// @brief An Option port, allows reading enum values
struct Option {
    uint8_t data;

    void write(uint8_t in) { data = in; }
    uint8_t read() const { return data; }
};
using OptionPort = Port<PortType::OPTION, Option>;

/// @brief Here we store all possible ports, which is used for generic interfaces.
using PortVariant = std::variant<
    ValuePort*, GatePort*, OptionPort*>;
}