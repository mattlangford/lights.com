#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <cstddef>
#include <stdexcept>

#include "node.hh"
#include "time.hh"

namespace runner {
class Runner {
private:
    struct NodeId;

public:
    template <typename T, typename...Args>
    NodeId add_node(std::string name, Args&&... args) {
        static_assert(std::is_base_of_v<Node, T>, "Nodes must derive from base class Node");
        return add_node(std::make_shared<T>(std::forward<Args>(args)...), std::move(name));
    }

    NodeId add_node(std::shared_ptr<Node> instance, std::string name);

    void connect(NodeId from_node, size_t from_output, NodeId to_node, size_t to_input);

    void run(Time now);

    void write(NodeId node, size_t input, float value);
    float read(NodeId node, size_t output);

    std::string dot() const;

private:
    void check_node_and_port(NodeId node, size_t* input, size_t* output);

    struct NodeId { size_t index; };
    struct Wrapper {
        std::string name;
        std::shared_ptr<Node> node;
        std::vector<size_t> inputs;
        std::vector<size_t> outputs;
    };
    inline static constexpr size_t INVALID_INPUT = -1;
    std::vector<Wrapper> wrappers_;
    std::vector<float> values_;

    std::optional<Time> previous_;
};
}