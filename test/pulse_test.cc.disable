#include "runner.hh"
#include "pulse.hh"
#include "utils.hh"
#include "time.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(PulseTest, PulseTest) {
    runner::Runner runner;

    constexpr float FADE_START = 9.1; 
    constexpr float DURATION_ON = 1.3; 
    constexpr float DURATION_OFF = 10.3; 
    constexpr float MIN = 3.0; 
    constexpr float MAX = 6.0; 

    auto pulse = runner.add_node<effects::Pulse>("pulse", DURATION_ON, DURATION_OFF, MIN, MAX);
    auto constant = runner.add_node<runner::Constant>("constant", FADE_START);

    runner.connect(constant, 0, pulse, 0);

    runner.run(runner::from_seconds(0.0));

    EXPECT_FLOAT_EQ(runner.read(pulse, 0), MIN);

    runner.run(runner::from_seconds(FADE_START + 0.1));

    float value_up0 = runner.read(pulse, 0);
    EXPECT_GT(value_up0, MIN);
    EXPECT_LT(value_up0, MAX);

    runner.run(runner::from_seconds(FADE_START + 0.2));
    float value_up1 = runner.read(pulse, 0);
    EXPECT_GT(value_up1, MIN);
    EXPECT_LT(value_up1, MAX);
    EXPECT_GT(value_up1, value_up0);

    runner.run(runner::from_seconds(FADE_START + DURATION_ON));
    EXPECT_FLOAT_EQ(runner.read(pulse, 0), MAX);

    runner.run(runner::from_seconds(FADE_START + DURATION_ON + 0.1));
    float value_down0 = runner.read(pulse, 0);
    EXPECT_GT(value_down0, MIN);
    EXPECT_LT(value_down0, MAX);

    runner.run(runner::from_seconds(FADE_START + DURATION_ON + 0.2));
    float value_down1 = runner.read(pulse, 0);
    EXPECT_GT(value_down0, MIN);
    EXPECT_LT(value_down1, MAX);
    EXPECT_LT(value_down1, value_down0);

    // The up rate should be faster than the down rate
    EXPECT_GT(abs(value_up1 - value_up0), abs(value_down1 - value_down0));
}