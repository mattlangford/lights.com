#pragma once

#include "context.hh"
#include "port.hh"

#include <tuple>

namespace runner {

class Node {
public:
    virtual ~Node() = default;

    virtual size_t input_count() const { return 0; }
    virtual size_t output_count() const { return 0; }

    virtual void callback(Context& context) = 0;
};

class NodeBase {
public:
    virtual ~NodeBase() = default;

    /// @brief Fetch the (generic) input and output ports.
    virtual std::vector<PortVariant> inputs() = 0;
    virtual std::vector<PortVariant> outputs() = 0;

    /// @brief Conenct the (generic) input from the other node to the specific output port of this node.
    virtual bool connect(uint8_t output_port, PortVariant& input) = 0;

    /// @brief Invoked by the runner.
    virtual void callback() = 0;
};

template<typename InTuple , typename OutTuple>
class TypedNode : public NodeBase {
public:
    TypedNode(TypedNode&) = delete;
    TypedNode(TypedNode&&) = delete;
    TypedNode& operator=(TypedNode&) = delete;
    TypedNode& operator=(TypedNode&&) = delete;

    std::vector<PortVariant> inputs() override { return to_port_variant_vector(inputs_); }
    std::vector<PortVariant> outputs() override { return to_port_variant_vector(outputs_); }

    bool connect(uint8_t output_port, PortVariant& input) override {
        return set_input_port();
    }

protected:
    template <uint8_t Port>
    const auto read() const {
        auto& storage = std::get<Port>(inputs_).storage;
        std::optional<decltype(storage->read())> result;
        if (storage != nullptr) {
            result = storage->read();
        }
        return result;
    }

    template <uint8_t Port, typename T>
    const auto read_or(T default_value) const {
        return read<Port>().value_or(default_value);
    }

    template <uint8_t Port, typename...Args>
    void write(Args&&... out) {
        std::get<Port>(storage_).write(std::forward<Args>(out)...);
    }

private:
    template <class Tuple, std::size_t... Is>
    static constexpr auto to_storage_tuple_impl(std::index_sequence<Is...>) {
        return std::tuple{typename std::tuple_element_t<Is, Tuple>::Storage{}...};
    }

    template <class Tuple>
    static constexpr auto to_storage_tuple() {
        return to_storage_tuple_impl<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }

    template<class... Ts>
    std::vector<PortVariant> to_port_variant_vector(std::tuple<Ts...>& t) {
        std::vector<PortVariant> v;
        v.reserve(sizeof...(Ts));
        std::apply([&](auto&... e){ (v.emplace_back(&e), ...); }, t);
        return v;
    }

    template<uint8_t I = 0>
    bool set_input_port(uint8_t output_port, PortVariant& input) {
        if constexpr (I < std::tuple_size_v<OutTuple>) {
            using OutPortT = std::tuple_element_t<I, OutTuple>;
            if (output_port != I) {
                // Continue to iterate through the tuple
                return set_input_port<I + 1>(output_port, input);
            }
            auto* port = std::get_if<OutPortT*>(input);
            if (port == nullptr) {
                return false; // Mismatched type!
            }
            port->storage = &std::get<I>(storage_);
        }
        return false; // Invalid output port!
    }

private:
    // We store our own outputs.
    decltype(to_storage_tuple<InTuple>()) storage_ = to_storage_tuple<InTuple>();

    InTuple inputs_;
    OutTuple outputs_;
};

}