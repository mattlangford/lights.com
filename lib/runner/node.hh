#pragma once

#include "context.hh"
#include "port.hh"
#include <string>

namespace runner {

class Node {
public:
    virtual ~Node() = default;

    virtual size_t input_count() const { return 0; }
    virtual size_t output_count() const { return 0; }

    virtual void callback(Context& context) = 0;
};

template <typename...Ts>
class Inputs {
public:
    template <uint8_t Port>
    const auto& read() const {
        return std::get<Port>(ports_).read();
    }
    template <uint8_t Port, typename Enum>
    Enum read() const {
        return std::get<Port>(ports_).template read<Enum>();
    }

private:
    std::tuple<Ts...> ports_;
};

template <typename...Ts>
class Outputs {
public:
    template <uint8_t Port, typename T>
    void write(T out) {
        if (auto ptr = std::get<Port>(ports_)) {
            ptr->write(out);
        }
    }

    template <uint8_t Port>
    bool writeable() const {
        return std::get<Port>(ports_) == nullptr;
    }

private:
    std::tuple<Ts*...> ports_;
};
}