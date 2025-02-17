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

class Ports {
public:
    void set(size_t port, float value) const { values_->at(outputs_.at(port)) = value; }
    float get(size_t port) const { return values_->at(inputs_.at(port)); }
private:
    friend class Runner;
    std::vector<float>* values_;
    std::vector<size_t> inputs_;
    std::vector<size_t> outputs_;
};

class Runner {
private:
    struct Node {
        std::string name;
        size_t index;
        size_t inputs;
        size_t outputs;
        std::function<void(Ports&)> callback;
    };
    struct NodeId { size_t index; };
    static constexpr size_t INVALID_INPUT = -1;

public:
    NodeId add_node(std::string name, size_t inputs, size_t outputs, std::function<void(Ports&)> callback) {
        size_t index = nodes_.size();
        nodes_.push_back({
            .name = std::move(name),
            .index = index,
            .inputs=inputs,
            .outputs=outputs,
            .callback=std::move(callback)
        });

        auto& ports = ports_.emplace_back();
        ports.values_ = &values_;
        for (size_t out = 0; out < outputs; out++) {
            ports.outputs_.push_back(values_.size());
            values_.push_back(0.0);
        }
        for (size_t in = 0; in < inputs; in++) {
            ports.inputs_.push_back(INVALID_INPUT);
        }

        return {index};
    }

    void connect(NodeId from_node, size_t from_output, NodeId to_node, size_t to_input) {
        if (ports_.size() != nodes_.size()) throw std::logic_error("Ports and node sizes don't match");
        if (from_node.index >= nodes_.size()) throw std::runtime_error(std::format("Unable to load output node index={}", from_node.index));
        if (from_output >= ports_.at(from_node.index).outputs_.size()) throw std::runtime_error(std::format("Unable to load node output={} from node='{}'", from_output, nodes_.at(from_node.index).name));
        if (to_node.index >= nodes_.size()) throw std::runtime_error(std::format("Unable to load input node index={}", to_node.index));
        if (to_input >= ports_.at(to_node.index).inputs_.size()) throw std::runtime_error(std::format("Unable to load node input={} from node='{}'", to_input, nodes_.at(to_node.index).name));

        const auto& from_outputs = ports_.at(from_node.index).outputs_;
        auto& to_inputs = ports_.at(to_node.index).inputs_;
        to_inputs.at(to_input) = from_outputs.at(from_output);
    }

    void validate() {
        for (size_t i = 0; i < ports_.size(); ++i)
            for (size_t in = 0; in < ports_[i].inputs_.size(); ++in)
                if (ports_[i].inputs_[in] == INVALID_INPUT)
                    throw std::runtime_error(
                        std::format("Node '{}' is missing input {}", nodes_.at(i).name, in));
    }

    void run() {
        validate();
        for (size_t i = 0; i < nodes_.size(); ++i) {
            nodes_[i].callback(ports_[i]);
        }
    }

    std::string dot() const {
        std::map<size_t, std::vector<std::string>> inputs_to_nodes;
        for (const auto& node : nodes_) {
            for (size_t input : ports_.at(node.index).inputs_) {
                inputs_to_nodes[input].push_back(node.name);
            }
        }

        std::stringstream ss;
        ss << "digraph runner {\n";
        for (const auto& node : nodes_)
            for (size_t output : ports_.at(node.index).outputs_)
                for (const auto& name : inputs_to_nodes.at(output))
                    ss << "  " << node.name << " -> " << name << " [label=value" << output << "]\n";

        ss << "}";
        return ss.str();
    }

private:
    std::vector<Node> nodes_;
    std::vector<Ports> ports_;
    std::vector<float> values_;
};
