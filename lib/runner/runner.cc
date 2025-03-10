#include <cstddef>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <map>

#include "runner.hh"
#include "context.hh"
#include "log.hh"

namespace runner {

Runner::NodeId Runner::add_node(std::shared_ptr<Node> instance, std::string name) {
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
}

Runner::NodeId Runner::id_from_name(const std::string& name) const {
    for (size_t i = 0; i < wrappers_.size(); ++i) {
        if (wrappers_[i].name == name) {
            return {.index=i};
        }
    }
    return {.index=INVALID_INPUT};
}

void Runner::check_node_and_port(NodeId node, size_t* input, size_t* output) {
    CHECK(node.index < wrappers_.size(), "Invalid node index %zu", node.index);
    if (input) {
        auto& inputs = wrappers_[node.index].inputs;
        CHECK(*input < inputs.size(), "Invalid input=%zu from node='%s'", *input, wrappers_[node.index].name.data());
        size_t input_index = inputs[*input];
        CHECK(input_index == INVALID_INPUT || input_index < values_.size(), "Invalid access into values %zu >= %zu", inputs[*input], values_.size());
    }
    if (output) {
        auto& outputs = wrappers_[node.index].outputs;
        CHECK(*output < outputs.size(), "Invalid output=%zu from node='%s'", *output, wrappers_[node.index].name.data());
        CHECK(outputs[*output] < values_.size(), "Invalid access into values %zu >= %zu", outputs[*output], values_.size());
    }
}
}