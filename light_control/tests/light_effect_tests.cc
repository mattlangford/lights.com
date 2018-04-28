#include "dmx_control/light_effects.hh"
#include <assert.h>
#include <iostream>

//////////////////////////////////////////////
/// Helpers //////////////////////////////////
//////////////////////////////////////////////

inline bool colors_are_equal(const Color& lhs, const Color& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

struct test_fixture
{
    Color c0 {0, 0, 0};
    Color c1 {200, 100, 0};
};

static test_fixture fixture {};

#define RUN_TEST(test_name) \
    std::cout << " [ RUNNING ]: " << #test_name << "\n"; \
    test_linear_fade(fixture); \
    std::cout << " [ PASSED  ]: " << #test_name << "\n"; \


//////////////////////////////////////////////
/// Tests ////////////////////////////////////
//////////////////////////////////////////////

void test_linear_fade(test_fixture f)
{
    constexpr uint8_t num_steps = 10;
    const auto linear = generate_transition<num_steps>(f.c0, f.c1);

    //
    // Since they're evenly spaced, this should be one step (NOT the starting color)
    //
    constexpr uint8_t r_first_step = 200 / num_steps;
    constexpr uint8_t g_first_step = 100 / num_steps;
    constexpr uint8_t b_first_step = 0 / num_steps;
    assert(colors_are_equal(linear.front(), {r_first_step, g_first_step, b_first_step}));

    //
    // The last color should always be the ending one we gave
    //
    assert(colors_are_equal(linear.back(), f.c1));
}

//
// ############################################################################
//

void test_smooth_fade(test_fixture f)
{
    constexpr uint8_t num_steps = 0;
    const auto smooth = generate_transition<num_steps, smooth_transition_strategy>(f.c0, f.c1);

    //
    // The middlemost element should be the average of the two input colors
    //
    assert(colors_are_equal(linear[num_steps / 2], {100, 50, 0}));

    //
    // The last color should always be the ending one we gave
    //
    assert(colors_are_equal(linear.back(), f.c1));
}

int main()
{
    RUN_TEST(test_linear_fade);
    //RUN_TEST(test_smooth_fade);
    test_smooth_fade(fixture);
}
