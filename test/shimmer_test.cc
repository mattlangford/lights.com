#include "runner.hh"
#include "shimmer.hh"
#include "utils.hh"
#include "time.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(ShimmerTest, WithinMinMax) {
    runner::Runner runner;
    constexpr float MIN = 3.0; 
    constexpr float MAX = 6.0; 

    auto shimmer = runner.add_node<effects::Shimmer>("shimmer", 0.0, 10.0, 3, MIN, MAX);

    runner.run(runner::from_seconds(0));

    float previous = 0.0;
    for (size_t i = 1; i < 10; ++i) {
        runner.run(runner::from_seconds(i));
        float value = runner.read(shimmer, 0);
        ASSERT_GT(value, MIN);
        ASSERT_LT(value, MAX);
        ASSERT_NE(value, previous);
        previous = value;
    }
}