#pragma once
#include <variant>
#include <vector>
#include <string>

namespace config {

struct AdderNode { uint8_t inputs = 2; };
struct SubtractorNode { uint8_t inputs = 2; };
struct MultiplierNode { uint8_t inputs = 2; };
struct DividerNode { uint8_t inputs = 2; };

struct Node {
    std::string name;
    std::variant<
        AdderNode,
        SubtractorNode,
        MultiplierNode,
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