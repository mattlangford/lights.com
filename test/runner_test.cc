#include "runner.hh"
#include "node.hh"

#include <gtest/gtest.h>

class Constants final : public Node {
public:
    Constants(float v0, float v1) : v0(v0), v1(v1) { }
    size_t output_count() const override { return 2; }
    bool callback(Context& context) override {
        context.output(0, v0);
        context.output(1, v1);
        return true;
    }

private:
    float v0, v1;
};

class Adder final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 1; }
    bool callback(Context& context) override  {
        context.output(0, context.input(0) + context.input(1));
        return true;
    }
};

class Subtractor final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 1; }
    bool callback(Context& context) override {
        context.output(0, context.input(0) - context.input(1));
        return true;
    }
};

class Saver final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 0; }
    bool callback(Context& context) override {
        v0 = context.input(0);
        v1 = context.input(1);
        time = context.now();
        return true;
    }

    float v0;
    float v1;
    uint32_t time;
};

TEST(RunnerTest, BasicRunner) {
    Runner runner;

    constexpr float IN0 = 1.1;
    constexpr float IN1 = 3.0;
    const auto constants = runner.add_node<Constants>("constants", IN0, IN1);
    const auto adder = runner.add_node<Adder>("adder");
    const auto subtractor = runner.add_node<Subtractor>("subtractor");
    auto saver_ptr = std::make_shared<Saver>();
    const auto saver = runner.add_node(saver_ptr, "saver");

    runner.connect(constants, 0, adder, 0);
    runner.connect(constants, 1, adder, 1);
    runner.connect(constants, 0, subtractor, 0);
    runner.connect(constants, 1, subtractor, 1);
    runner.connect(adder, 0, saver, 0);
    runner.connect(subtractor, 0, saver, 1);

    runner.run(123);

    EXPECT_FLOAT_EQ(saver_ptr->v0, 4.1);
    EXPECT_FLOAT_EQ(saver_ptr->v1, -1.9);
    EXPECT_EQ(saver_ptr->time, 123);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
