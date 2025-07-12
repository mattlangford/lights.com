#pragma once

#include "node.hh"
#include "port.hh"
#include "config.hh"

namespace runner {
class OpNode final : public TypedNode<
    std::tuple<ValuePort, ValuePort, OptionPort>, std::tuple<ValuePort>> {

public:
    enum Ops : uint8_t {
        ADD = 0,
        SUBTRACT,
        MULTIPLY,
        SATURATE_ADD,
        SATURATE_SUBTRACT,
        UNKNOWN
    };

    OpNode(Ops default_op = Ops::ADD, std::string_view name = "OpNode");

    void callback() override;

private:
    const Ops default_op_;
};
}