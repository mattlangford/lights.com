#include "runner.hh"
#include "utils.hh"
#include "universe.hh"
#include "time.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(UniverseTest, SimpleLights) {
    runner::Runner runner;

    auto constant = runner.add_node<runner::Constant>("constant", std::vector{1.f, 0.5f, 0.1f});

    std::shared_ptr<light::Universe> universe_ptr = std::make_shared<light::Universe>();
    universe_ptr->add_light({.name="test", .channels={0, 2, 3}});
    universe_ptr->add_light({.name="test2", .channels={5, 6, 7}});
    auto universe = runner.add_node(universe_ptr, "universe");

    runner.connect(constant, 0, universe, 0);
    runner.connect(constant, 1, universe, 1);
    runner.connect(constant, 2, universe, 2);

    runner.connect(constant, 0, universe, 3);
    runner.connect(constant, 1, universe, 4);
    runner.connect(constant, 2, universe, 5);

    runner.run(runner::from_seconds(0.0));

    EXPECT_THAT(universe_ptr->channels(), testing::ElementsAre(
        255, 0, 127, 25, 0, 255, 127, 25));
}