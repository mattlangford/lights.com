#include "runner.hh"
#include "shimmer.hh"
#include "dijector.hh"
#include "time.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(ShimmerTest, WithinMinMax) {
    runner::Runner runner;
    constexpr float MIN = 3.0; 
    constexpr float MAX = 6.0; 

    auto shimmer = runner.add_node<effects::Shimmer>("shimmer", 0.0, 10.0, 3, MIN, MAX);
    auto saver_ptr = std::make_shared<runner::Saver<1>>();
    auto saver = runner.add_node(saver_ptr, "saver");

    runner.connect(shimmer, 0, saver, 0);

    runner.run(runner::from_seconds(0));

    float previous = 0.0;
    for (size_t i = 1; i < 10; ++i) {
        runner.run(runner::from_seconds(i));
        float value = saver_ptr->value<0>();
        ASSERT_GT(value, MIN);
        ASSERT_LT(value, MAX);
        ASSERT_NE(value, previous);
        previous = value;
    }
}