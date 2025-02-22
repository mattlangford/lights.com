#include "runner.hh"
#include "fade.hh"
#include "dijector.hh"
#include "time.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(FadeTest, BasicFade) {
    runner::Runner runner;
    constexpr float FADE_START = 9.1; 
    constexpr float DURATION = 1.3; 
    constexpr float MIN = 3.0; 
    constexpr float MAX = 6.0; 

    auto constant = runner.add_node<runner::Constant>("constant", FADE_START);
    auto fade = runner.add_node<effects::Fade>("fade", DURATION, MIN, MAX);

    runner.connect(constant, 0, fade, 0);

    runner.run(runner::from_seconds(0.0f));

    EXPECT_FLOAT_EQ(runner.read(fade, 0), MIN);

    runner.run(runner::from_seconds(FADE_START + 0.1));

    float value = runner.read(fade, 0);
    EXPECT_GT(value, MIN);
    EXPECT_LT(value, MAX);

    runner.run(runner::from_seconds(FADE_START + 0.2));
    EXPECT_GT(runner.read(fade, 0), MIN);
    EXPECT_LT(runner.read(fade, 0), MAX);
    EXPECT_GT(runner.read(fade, 0), value);

    runner.run(runner::from_seconds(FADE_START + 2 * DURATION));

    EXPECT_FLOAT_EQ(runner.read(fade, 0), MAX);
}