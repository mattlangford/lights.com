#include "config.hh"
#include "math.hh"
#include "factory.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(FactoryTest, BuildAdder) {
    config::Node node_config;
    node_config.name = "adder";
    node_config.config = config::AdderNode{3};

    auto ptr = runner::build(node_config);
    ASSERT_NE(ptr, nullptr);
    EXPECT_NE(std::dynamic_pointer_cast<runner::Adder>(ptr), nullptr);
}

TEST(FactoryTest, BuildRunner) {
    config::Runner runner_config;
    runner_config.nodes.push_back({
        .name = "constant",
        .config = config::ConstantNode{.values={1.1, 2.2, 3.3}},
    });
    runner_config.nodes.push_back({
        .name = "adder",
        .config = config::AdderNode{3},
    });
    runner_config.nodes.push_back({
        .name = "multiplier",
        .config = config::MultiplierNode{2},
    });

    runner_config.connections.push_back({
        .from_node = 0, .from_output = 0,
        .to_node = 1, .to_input = 0,
    });
    runner_config.connections.push_back({
        .from_node = 0, .from_output = 1,
        .to_node = 1, .to_input = 1,
    });
    runner_config.connections.push_back({
        .from_node = 0, .from_output = 2,
        .to_node = 1, .to_input = 2,
    });
    runner_config.connections.push_back({
        .from_node = 0, .from_output = 0,
        .to_node = 2, .to_input = 0,
    });
    runner_config.connections.push_back({
        .from_node = 0, .from_output = 1,
        .to_node = 2, .to_input = 1,
    });

    runner::Runner runner = runner::build(runner_config);
    std::cout << runner.dot() << "\n";
    runner.run({});
    EXPECT_FLOAT_EQ(runner.read(runner.id_from_name("adder"), 0), 6.6);
    EXPECT_FLOAT_EQ(runner.read(runner.id_from_name("multiplier"), 0), 2.42);
}