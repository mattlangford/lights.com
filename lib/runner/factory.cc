#include "node.hh"
#include "log.hh"
#include "factory.hh"
#include "math.hh"

namespace runner {
namespace {
struct Visitor {
    std::shared_ptr<Node> operator()(const config::AdderNode& adder) const { return std::make_shared<Adder>(); }

    template <typename T>
    std::shared_ptr<Node> operator()(const T& err) const {
        LOG_ERROR("Unknown config!");
        return nullptr;
    }
};
}

std::shared_ptr<Node> build(const config::Node& node_config) {
    return std::visit(Visitor{}, node_config.config);
}
}