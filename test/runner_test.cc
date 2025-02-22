#include "runner.hh"
#include "node.hh"
#include "dijector.hh"
#include "math.hh"

#include <gtest/gtest.h>

namespace runner {

TEST(RunnerTest, BasicRunner) {
    Runner runner;

    constexpr float IN0 = 1.1;
    constexpr float IN1 = 3.0;
    const auto constants = runner.add_node<Constant<2>>("constants", std::array{IN0, IN1});
    const auto adder = runner.add_node<Adder>("adder");
    const auto subtractor = runner.add_node<Subtractor>("subtractor");
    auto saver_ptr = std::make_shared<Saver<2>>();
    const auto saver = runner.add_node(saver_ptr, "saver");

    runner.connect(constants, 0, adder, 0);
    runner.connect(constants, 1, adder, 1);
    runner.connect(constants, 0, subtractor, 0);
    runner.connect(constants, 1, subtractor, 1);
    runner.connect(adder, 0, saver, 0);
    runner.connect(subtractor, 0, saver, 1);

    auto time = from_seconds(123.f);
    runner.run(time);

    EXPECT_FLOAT_EQ(saver_ptr->value<0>(), 4.1);
    EXPECT_FLOAT_EQ(saver_ptr->value<1>(), -1.9);
    EXPECT_EQ(saver_ptr->time, time);
}

class TestNode : public Node {
public:
    TestNode(size_t in, size_t out) : inputs_(in), outputs_(out) { }
    size_t input_count() const override { return inputs_; }
    size_t output_count() const override { return outputs_; }
    void callback(runner::Context& context) override {}

private:
    size_t inputs_;
    size_t outputs_;
};

TEST(RunnerTest, NoInputs) {
    Runner runner;
    auto n0 = runner.add_node<TestNode>("test", 0, 1);
    auto n1 = runner.add_node<TestNode>("test2", 0, 1);
    EXPECT_ANY_THROW(runner.connect(n0, 0, n1, 0));
}

TEST(RunnerTest, NoOutputs) {
    Runner runner;
    auto n0 = runner.add_node<TestNode>("test", 0, 0);
    auto n1 = runner.add_node<TestNode>("test2", 1, 1);
    EXPECT_ANY_THROW(runner.connect(n0, 0, n1, 0));
}

TEST(RunnerTest, InvalidNode) {
    Runner runner;
    auto n0 = runner.add_node<TestNode>("test", 1, 1);
    EXPECT_ANY_THROW(runner.connect({.index=500}, 0, n0, 0));
}
}
