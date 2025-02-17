#include "runner.hh"

#include <gtest/gtest.h>

TEST(RunnerTest, BasicRunner) {
    Runner runner;

    float output[2] = {0.0, 0.0};

    auto constants = runner.add_node("constatnts", 0, 2, [](Ports& ports){
        ports.set(0, 1.0);
        ports.set(1, 3.1);
    });

    auto adder = runner.add_node("adder", 2, 1, [](Ports& ports){
        ports.set(0, ports.get(0) + ports.get(1));
    });
    auto subtractor = runner.add_node("subtractor", 2, 1, [](Ports& ports){
        ports.set(0, ports.get(0) - ports.get(1));
    });

    auto printer1 = runner.add_node("printer1", 1, 0, [&](Ports& ports){
        output[0] = ports.get(0);
    });
    auto printer2 = runner.add_node("printer2", 1, 0, [&](Ports& ports){
        output[1] = ports.get(0);
    });

    runner.connect(constants, 0, adder, 0);
    runner.connect(constants, 1, adder, 1);
    runner.connect(constants, 0, subtractor, 0);
    runner.connect(constants, 1, subtractor, 1);
    runner.connect(adder, 0, printer1, 0);
    runner.connect(subtractor, 0, printer2, 0);

    runner.run();
    EXPECT_FLOAT_EQ(output[0], 4.1);
    EXPECT_FLOAT_EQ(output[1], -2.1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
