#include "math.hh"
#include "log.hh"

namespace runner {

OpNode::OpNode(Ops default_op, std::string_view name) : TypedNode(name), default_op_(default_op) { }

void OpNode::callback() {
    float lhs = read_or<0>(0.0);
    float rhs = read_or<1>(0.0);
    switch (read_or<2, Ops>(default_op_)) {
    case ADD:
        write<0>(lhs + rhs);
        return;
    case SUBTRACT:
        write<0>(lhs - rhs);
        return;
    case MULTIPLY:
        write<0>(lhs * rhs);
        return;
    case SATURATE_ADD:
        write<0>(std::clamp(lhs + rhs, 0.f, 1.f));
        return;
    case SATURATE_SUBTRACT:
        write<0>(std::clamp(lhs - rhs, 0.f, 1.f));
    default:
        break;
    }
}
}