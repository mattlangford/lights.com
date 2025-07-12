#include <cstddef>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <map>

#include "runner.hh"
#include "log.hh"

namespace runner {

Runner::NodeId Runner::add_node(std::unique_ptr<NodeBase> instance) {
    const NodeId id{.index = nodes_.size()};
    nodes_.emplace_back(std::move(instance));
    return id;
}

bool Runner::connect(NodeId from_node, uint8_t from_output, NodeId to_node, uint8_t to_input) {
    if (to_node.index >= nodes_.size()) { return false; }
    if (from_node.index >= nodes_.size()) { return false; }

    std::vector<PortVariant> input_ports = nodes_[to_node.index]->inputs();
    if (to_input >= input_ports.size()) { return false; }

    return nodes_[from_node.index]->connect(from_output, input_ports[to_input]);
}

void Runner::tick(Time now) {
    Duration dt = now - previous_.value_or(now);
    for (auto& node : nodes_) {
        node->callback();
    }
    previous_ = now;
}

std::string Runner::dot() const {
    /*
    std::map<size_t, std::vector<std::string>> inputs_to_nodes;
    for (const auto& wrapper : wrappers_) {
        for (size_t input : wrapper.inputs) {
            inputs_to_nodes[input].push_back(wrapper.name);
        }
    }

    std::string s;
    s + "digraph runner {\n";
    for (const auto& wrapper : wrappers_)
        for (size_t output : wrapper.outputs) {
            auto it = inputs_to_nodes.find(output);
            if (it != inputs_to_nodes.end())
                for (const auto& name : it->second)
                    s += std::string("  ") + wrapper.name + " -> " + name + " [label=value" + std::to_string(output) + "]\n";
        }

    s + "}";
    return s;
    */
   return "N/A";
}

}