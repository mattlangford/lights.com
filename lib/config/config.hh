#pragma once
#include <variant>
#include <vector>
#include <string>

namespace config {

struct ConstantNode { std::vector<float> values; };

struct MathConfig {
    bool saturating = false;
    uint8_t inputs = 2;
};

struct AdderNode : public MathConfig {};
struct SubtractorNode : public MathConfig {};
struct MultiplierNode : public MathConfig {};
struct DividerNode : public MathConfig {};

struct UniverseNode {
    struct Light {
        std::string name;
        std::vector<uint16_t> channels;
    };
    std::vector<Light> lights;
    uint16_t max_channel;
};

struct Node {
    std::string name;
    std::variant<
        ConstantNode,
        AdderNode,
        SubtractorNode,
        MultiplierNode,
        DividerNode,
        UniverseNode
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