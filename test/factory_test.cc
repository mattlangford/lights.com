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