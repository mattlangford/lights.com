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

    auto fade = runner.add_node<effects::Fade>("fade", DURATION, MIN, MAX);
    auto constant = runner.add_node<runner::Constant<1>>("constant", std::array{FADE_START});
    auto saver_ptr = std::make_shared<runner::Saver<1>>();
    auto saver = runner.add_node(saver_ptr, "saver");

    runner.connect(constant, 0, fade, 0);
    runner.connect(fade, 0, saver, 0);

    runner.run(runner::from_seconds(0.0f));

    EXPECT_FLOAT_EQ(saver_ptr->value<0>(), MIN);

    runner.run(runner::from_seconds(FADE_START + 0.1));

    float value = saver_ptr->value<0>();
    EXPECT_GT(value, MIN);
    EXPECT_LT(value, MAX);

    runner.run(runner::from_seconds(FADE_START + 0.2));
    EXPECT_GT(saver_ptr->value<0>(), MIN);
    EXPECT_LT(saver_ptr->value<0>(), MAX);
    EXPECT_GT(saver_ptr->value<0>(), value);

    runner.run(runner::from_seconds(FADE_START + 2 * DURATION));

    EXPECT_FLOAT_EQ(saver_ptr->value<0>(), MAX);
}