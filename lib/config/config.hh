#pragma once
#include <variant>
#include <vector>
#include <string>

namespace config {

struct AdderNode { size_t inputs = 2; };
struct SubtractorNode { size_t inputs = 2; };
struct MultiplerNode { size_t inputs = 2; };
struct DividerNode { size_t inputs = 2; };

struct Node {
    std::string name;
    std::variant<
        AdderNode,
        SubtractorNode,
        MultiplerNode,
        DividerNode
    > config;
};

struct Connection {
    size_t input_node;
    size_t output_port;

    size_t output_node;
    size_t input_port;
};

struct Runner {
    std::vector<Node> nodes;
    std::vector<Connection> connections;
};

}