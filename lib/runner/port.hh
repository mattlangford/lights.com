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

struct ValuePort : PortTag {
    using Storage = float;
    static constexpr PortType type = PortType::VALUE;
    std::string_view name;
    Storage data;

    void write(Storage in) { data = in; }
    const Storage& read() const { return data; }
};
using Value = ValuePort::Storage;

struct GatePort : PortTag {
    using Storage = Time;
    static constexpr PortType type = PortType::GATE;
    std::string_view name;
    Storage trigger{std::chrono::seconds(0)};

    void write() { trigger = std::chrono::steady_clock::now(); }
    const Storage& read() const { return trigger; }
};
using Gate = GatePort::Storage;

struct OptionPort : PortTag {
    using Storage = uint8_t;
    static constexpr PortType type = PortType::OPTION;
    std::string_view name;
    Storage data;

    void write(Storage in) { data = in; }
    template <typename Enum>
    Enum read() const {
        static_assert(std::is_enum_v<Enum> && std::is_same_v<std::underlying_type_t<Enum>, Storage>, "Can only read Enum options.");
        return static_cast<Enum>(data);
    }
};
using Option = OptionPort::Storage;

}