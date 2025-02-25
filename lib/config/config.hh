#pragma once
#include <variant>
#include <vector>
#include <string>

namespace config {

struct ConstantNode { std::vector<float> values; };
struct AdderNode { uint8_t inputs = 2; };
struct SubtractorNode { uint8_t inputs = 2; };
struct MultiplierNode { uint8_t inputs = 2; };
struct DividerNode { uint8_t inputs = 2; };

struct Node {
    std::string name;
    std::variant<
        ConstantNode,
        AdderNode,
        SubtractorNode,
        MultiplierNode,
        DividerNode
    > config;
};

struct Connection {
    size_t from_node;
    uint8_t from_output;
    size_t to_node;
    uint8_t to_input;
};

struct Runner {
    std::vector<Node> nodes;
    std::vector<Connection> connections;
};

}