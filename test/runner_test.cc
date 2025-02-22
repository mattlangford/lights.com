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
    const auto constants = runner.add_node<Constant<2>>("constants_1", std::array{IN0, IN1});
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
}
