#pragma once

#include <vector>
#include <string_view>
#include <memory>
#include <cstddef>

#include "node.hh"
#include "time.hh"

namespace runner {
class Runner {
public:
    struct NodeId { size_t index; };

public:
    template <typename T, typename...Args>
    NodeId add_node(Args&&... args) {
        static_assert(std::is_base_of_v<NodeBase, T>, "Nodes must derive from base class NodeBase");
        return add_node(std::make_unique<T>(std::forward<Args>(args)...));
    }

    NodeId add_node(std::unique_ptr<NodeBase> instance);

    bool connect(NodeId from_node, uint8_t from_output, NodeId to_node, uint8_t to_input);

    void tick(Time now);

    std::string dot() const;

private:
    std::vector<std::unique_ptr<NodeBase>> nodes_;
    std::optional<Time> previous_;
};
}