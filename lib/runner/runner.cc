#include <cstddef>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <map>
#include <sstream>

#include "runner.hh"
#include "context.hh"

namespace runner {
auto Runner::add_node(std::shared_ptr<Node> instance, std::string name) -> NodeId {
    const NodeId id{.index = wrappers_.size() };
    Wrapper& wrapper = wrappers_.emplace_back();
    wrapper.name = std::move(name);
    wrapper.node = std::move(instance);
    for (size_t out = 0; out < wrapper.node->output_count(); out++) {
        wrapper.outputs.push_back(values_.size());
        values_.push_back(0.0);
    }
    for (size_t in = 0; in < wrapper.node->input_count(); in++) {
        wrapper.inputs.push_back(INVALID_INPUT);
    }
    return id;
}

void Runner::connect(NodeId from_node, size_t from_output, NodeId to_node, size_t to_input) {
    if (from_node.index >= wrappers_.size()) {
        std::stringstream ss;
        ss << "Unable to load output node index=" << from_node.index;
        throw std::runtime_error(ss.str());
    }
    if (from_output >= wrappers_.at(from_node.index).outputs.size()) {
        std::stringstream ss;
        ss << "Unable to load node output=" << from_output << " from node ='" << wrappers_.at(from_node.index).name << "'";
        throw std::runtime_error(ss.str());
    }
    if (to_node.index >= wrappers_.size()) {
        std::stringstream ss;
        ss << "Unable to load input node index=" << to_node.index;
        throw std::runtime_error(ss.str());
    }
    if (to_input >= wrappers_.at(to_node.index).inputs.size()) {
        std::stringstream ss;
        ss << "Unable to load node input=" << to_input << " from node ='" << wrappers_.at(to_node.index).name << "'";
        throw std::runtime_error(ss.str());
    }

    const auto& from_outputs = wrappers_.at(from_node.index).outputs;
    auto& to_inputs = wrappers_.at(to_node.index).inputs;
    to_inputs.at(to_input) = from_outputs.at(from_output);
}

void Runner::validate() const {
    for (const Wrapper& wrapper : wrappers_)
        for (size_t in = 0; in < wrapper.inputs.size(); ++in)
            if (wrapper.inputs[in] == INVALID_INPUT) {
                std::stringstream ss;
                ss << "Node '" << wrapper.name << "' has an unconnected input " << in;
                throw std::runtime_error(ss.str());
            }
}

void Runner::run(Time now) {
    validate();
    Duration dt = now - previous_.value_or(now);
    for (Wrapper& wrapper : wrappers_) {
        Context context(now, dt, values_, wrapper.inputs, wrapper.outputs);
        wrapper.node->callback(context);
    }
}

std::string Runner::dot() const {
    std::map<size_t, std::vector<std::string>> inputs_to_nodes;
    for (const auto& wrapper : wrappers_) {
        for (size_t input : wrapper.inputs) {
            inputs_to_nodes[input].push_back(wrapper.name);
        }
    }

    std::stringstream ss;
    ss << "digraph runner {\n";
    for (const auto& wrapper : wrappers_)
        for (size_t output : wrapper.outputs)
            for (const auto& name : inputs_to_nodes.at(output))
                ss << "  " << wrapper.name << " -> " << name << " [label=value" << output << "]\n";

    ss << "}";
    return ss.str();
}
}