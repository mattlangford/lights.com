#include <cstddef>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <map>
#include <format>

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
    check_node_and_port(from_node, nullptr, &from_output);
    check_node_and_port(to_node, &to_input, nullptr);
    const auto& from_outputs = wrappers_.at(from_node.index).outputs;
    auto& to_inputs = wrappers_.at(to_node.index).inputs;
    to_inputs.at(to_input) = from_outputs.at(from_output);
}

void Runner::run(Time now) {
    Duration dt = now - previous_.value_or(now);
    for (Wrapper& wrapper : wrappers_) {
        Context context(now, dt, values_, wrapper.inputs, wrapper.outputs);
        wrapper.node->callback(context);
    }
    previous_ = now;
}

void Runner::write(NodeId node, size_t input, float value) {
    check_node_and_port(node, &input, nullptr);
    auto& inputs = wrappers_.at(node.index).inputs;
    values_.at(inputs.at(input)) = value;
}

float Runner::read(NodeId node, size_t output) {
    check_node_and_port(node, nullptr, &output);
    auto& outputs = wrappers_.at(node.index).outputs;
    return values_.at(outputs.at(output));
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

void Runner::check_node_and_port(NodeId node, size_t* input, size_t* output) {
    if (node.index >= wrappers_.size()) { throw std::runtime_error(std::format("Invalid node index={}", node.index)); }
    if (input) {
        auto& inputs = wrappers_[node.index].inputs;
        if (*input >= inputs.size()) { throw std::runtime_error(std::format("Invalid input={} from node='{}'", *input, wrappers_[node.index].name)); }
        size_t input_index = inputs[*input];
        if (input_index != INVALID_INPUT && inputs[*input] >= values_.size()) {
            throw std::logic_error(std::format("Invalid access into values {} >= {}", inputs[*input], values_.size()));
        }
    }
    if (output) {
        auto& outputs = wrappers_[node.index].outputs;
        if (*output >= outputs.size()) { throw std::runtime_error(std::format("Invalid output={} from node='{}'", *output, wrappers_[node.index].name)); }
        if (outputs[*output] >= values_.size()) { throw std::logic_error("Invalid access into values"); }
    }
}
}