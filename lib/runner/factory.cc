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

Runner build(const config::Runner& runner_config) {
    Runner runner;
    std::vector<Runner::NodeId> nodes;

    for (const config::Node& node_config : runner_config.nodes) {
        auto ptr = build(node_config);
        CHECK(ptr != nullptr, "Node '{}' has an invalid config.", node_config.name);
        nodes.push_back(runner.add_node(ptr, node_config.name));
    }

    for (const config::Connection& connection : runner_config.connections) {
        runner.connect(nodes.at(connection.input_node), connection.output_port, nodes.at(connection.output_node), connection.input_port);
    }

    return runner;
}
}