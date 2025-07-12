#include "runner.hh"
#include "node.hh"
#include "math.hh"
#include "log.hh"

#include <gtest/gtest.h>

namespace runner {

class Constant : public TypedNode<std::tuple<>, std::tuple<ValuePort>> {
public:
    Constant(float value, std::string_view name = "Constant") : TypedNode(name), value_(value) { }

    void callback() {
        write<0>(value_);
    }

private:
    float value_ = 0.0;
};

class Reader : public TypedNode<std::tuple<ValuePort>, std::tuple<>> {
public:
    Reader(float& output, std::string_view name = "Reader") : TypedNode(name), output_(output) { }

    void callback() {
        if (auto out = read<0>()) {
            output_ = *out;
        }
    }

private:
    float& output_;
};

TEST(RunnerTest, BasicRunner) {
    Runner runner;

    constexpr float IN0 = 1.1;
    constexpr float IN1 = 3.0;
    const auto constant0 = runner.add_node<Constant>(IN0, "constant0");
    const auto constant1 = runner.add_node<Constant>(IN1, "constant1");
    const auto adder = runner.add_node<OpNode>(OpNode::ADD, "adder");
    const auto subtractor = runner.add_node<OpNode>(OpNode::SUBTRACT, "subtractor");
    const auto multiplier = runner.add_node<OpNode>(OpNode::MULTIPLY, "mulitplier");

    float add_result = 0.0;
    float sub_result = 0.0;
    float mul_result = 0.0;
    const auto adder_result = runner.add_node<Reader>(add_result);
    const auto subtractor_result = runner.add_node<Reader>(sub_result);
    const auto multiplier_result = runner.add_node<Reader>(mul_result);

    ASSERT_TRUE(runner.connect(constant0, 0, adder, 0));
    ASSERT_TRUE(runner.connect(constant1, 0, adder, 1));
    ASSERT_TRUE(runner.connect(constant0, 0, subtractor, 0));
    ASSERT_TRUE(runner.connect(constant1, 0, subtractor, 1));
    ASSERT_TRUE(runner.connect(subtractor, 0, multiplier, 0));
    ASSERT_TRUE(runner.connect(constant1, 0, multiplier, 1));
    ASSERT_TRUE(runner.connect(adder, 0, adder_result, 0));
    ASSERT_TRUE(runner.connect(subtractor, 0, subtractor_result, 0));
    ASSERT_TRUE(runner.connect(multiplier, 0, multiplier_result, 0));

    auto time = from_seconds(123.f);
    runner.tick(time);

    EXPECT_FLOAT_EQ(add_result, 4.1);
    EXPECT_FLOAT_EQ(sub_result, -1.9);
    EXPECT_FLOAT_EQ(mul_result, -5.7);
}

}
