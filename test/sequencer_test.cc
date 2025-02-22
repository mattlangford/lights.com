#include "runner.hh"
#include "sequencer.hh"
#include "dijector.hh"
#include "time.hh"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(SequencerTest, RandomVsStep) {
    runner::Runner runner;

    constexpr std::array VALUES{1.f, 2.f, 3.f};

    auto trigger_ptr = std::make_shared<runner::Constant<1>>(std::array{0.f});
    auto trigger = runner.add_node(trigger_ptr, "trigger");
    auto constant = runner.add_node<runner::Constant<3>>("constant", VALUES);

    auto sequencer_step = runner.add_node<effects::Sequencer>(
        "sequencer_step", 3, effects::Sequencer::Mode::STEP);
    auto sequencer_rand = runner.add_node<effects::Sequencer>(
        "sequencer_rand", 3, effects::Sequencer::Mode::RANDOM, 42);

    auto saver_ptr = std::make_shared<runner::Saver<2>>();
    auto saver = runner.add_node(saver_ptr, "saver");

    runner.connect(trigger, 0, sequencer_step, 0);
    runner.connect(constant, 0, sequencer_step, 1);
    runner.connect(constant, 1, sequencer_step, 2);
    runner.connect(constant, 2, sequencer_step, 3);

    runner.connect(trigger, 0, sequencer_rand, 0);
    runner.connect(constant, 0, sequencer_rand, 1);
    runner.connect(constant, 1, sequencer_rand, 2);
    runner.connect(constant, 2, sequencer_rand, 3);

    runner.connect(sequencer_step, 0, saver, 0);
    runner.connect(sequencer_rand, 0, saver, 1);

    auto now = runner::from_seconds(0.0f);
    runner.run(now);

    const auto step = [&](){ return saver_ptr->value<0>(); };
    const auto rand = [&](){ return saver_ptr->value<1>(); };

    EXPECT_FLOAT_EQ(step(), VALUES[0]);
    EXPECT_FLOAT_EQ(rand(), VALUES[0]);

    // No trigger yet
    runner.run(now);
    EXPECT_FLOAT_EQ(step(), VALUES[0]);
    EXPECT_FLOAT_EQ(rand(), VALUES[0]);

    // Trigger!
    trigger_ptr->set<0>(1.0);
    runner.run(now);
    EXPECT_FLOAT_EQ(step(), VALUES[1]);
    EXPECT_FLOAT_EQ(rand(), VALUES[2]); // We know the seed, so this is hardcoded

    // Trigger is still high.
    runner.run(now);
    EXPECT_FLOAT_EQ(step(), VALUES[2]);
    EXPECT_FLOAT_EQ(rand(), VALUES[0]); // We know the seed, so this is hardcoded

}