#include "node.hh"
#include "log.hh"
#include "factory.hh"
#include "math.hh"
#include "utils.hh"

namespace runner {
namespace {
struct Visitor {
    std::shared_ptr<Node> operator()(const config::ConstantNode& c) const { return std::make_shared<Constant>(c); }
    std::shared_ptr<Node> operator()(const config::AdderNode& c) const { return std::make_shared<Adder>(c); }
    std::shared_ptr<Node> operator()(const config::SubtractorNode& c) const { return std::make_shared<Subtractor>(c); }
    std::shared_ptr<Node> operator()(const config::MultiplierNode& c) const { return std::make_shared<Multiplier>(c); }
    std::shared_ptr<Node> operator()(const config::DividerNode& c) const { return std::make_shared<Divider>(c); }

    template <typename T>
    std::shared_ptr<Node> operator()(const T& err) const {
        LERROR("Unknown config!");
        return nullptr;
    }
};
}

std::shared_ptr<Node> build(const config::Node& node_config) {
    return std::visit(Visitor{}, node_config.config);
}

Runner build(const config::Runner& runner_config) {
    Runner runner;
    std::vector<Runner::NodeId> nodes;

    for (const config::Node& node_config : runner_config.nodes) {
        auto ptr = build(node_config);
        CHECK(ptr != nullptr, "Node '%s' has an invalid config.", node_config.name.data());
        nodes.push_back(runner.add_node(ptr, node_config.name));
    }

    for (const config::Connection& connection : runner_config.connections) {
        runner.connect(nodes.at(connection.from_node), connection.from_output, nodes.at(connection.to_node), connection.to_input);
    }

    return runner;
}
}