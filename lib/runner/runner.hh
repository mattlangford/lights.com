#pragma once

#include <vector>
#include <string>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <format>
#include <iostream>
#include <map>
#include <sstream>

#include "node.hh"
#include "context.hh"

class Runner {
private:
    struct NodeId;

public:
    template <typename T, typename...Args>
    NodeId add_node(std::string name, Args&&... args) {
        static_assert(std::is_base_of_v<Node, T>, "Nodes must derive from base class Node");
        return add_node(std::make_shared<T>(std::forward<Args>(args)...), std::move(name));
    }

    NodeId add_node(std::shared_ptr<Node> instance, std::string name) {
        if (instance == nullptr) throw std::runtime_error("Can not add null node");
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

    void connect(NodeId from_node, size_t from_output, NodeId to_node, size_t to_input) {
        if (from_node.index >= wrappers_.size()) throw std::runtime_error(std::format("Unable to load output node index={}", from_node.index));
        if (from_output >= wrappers_.at(from_node.index).outputs.size()) throw std::runtime_error(std::format("Unable to load node output={} from node='{}'", from_output, wrappers_.at(from_node.index).name));
        if (to_node.index >= wrappers_.size()) throw std::runtime_error(std::format("Unable to load input node index={}", to_node.index));
        if (to_input >= wrappers_.at(to_node.index).inputs.size()) throw std::runtime_error(std::format("Unable to load node input={} from node='{}'", to_input, wrappers_.at(to_node.index).name));

        const auto& from_outputs = wrappers_.at(from_node.index).outputs;
        auto& to_inputs = wrappers_.at(to_node.index).inputs;
        to_inputs.at(to_input) = from_outputs.at(from_output);
    }

    void validate() const {
        for (const Wrapper& wrapper : wrappers_)
            for (size_t in = 0; in < wrapper.inputs.size(); ++in)
                if (wrapper.inputs[in] == INVALID_INPUT)
                    throw std::runtime_error(
                        std::format("Node '{}' is missing input {}", wrapper.name, in));
    }

    void run(uint32_t now) {
        validate();
        for (Wrapper& wrapper : wrappers_) {
            Context context(now, values_, wrapper.inputs, wrapper.outputs);
            wrapper.node->callback(context);
        }
    }

    std::string dot() const {
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

private:
    struct NodeId { size_t index; };
    struct Wrapper {
        std::string name;
        std::shared_ptr<Node> node;
        std::vector<size_t> inputs;
        std::vector<size_t> outputs;
    };
    static constexpr size_t INVALID_INPUT = -1;
    std::vector<Wrapper> wrappers_;
    std::vector<float> values_;

};
