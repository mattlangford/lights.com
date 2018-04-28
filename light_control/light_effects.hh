#pragma once

#include <array>
#include <cmath>
#include <ratio>
#include <ostream>
#include <iostream>
#include <stdint.h>

///
/// Color type with a few operators that help it be used (at compile time)
///
struct Color
{
    constexpr Color() = default;

    constexpr Color(uint8_t r_, uint8_t g_, uint8_t b_)
        : r(r_), g(g_), b(b_)
    {
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    constexpr Color operator+(const Color& rhs) const
    {
        return {static_cast<uint8_t>(r + rhs.r),
                static_cast<uint8_t>(g + rhs.g),
                static_cast<uint8_t>(b + rhs.b)};
    }

};

std::ostream& operator<<(std::ostream &o, const Color& c)
{
    o << "{ " << static_cast<uint16_t>(c.r)
        << ", " << static_cast<uint16_t>(c.g)
        << ", " << static_cast<uint16_t>(c.b)
        << " }";

    return o;
}

constexpr Color operator*(const double& d, const Color &c)
{
    return {static_cast<uint8_t>(d * c.r),
            static_cast<uint8_t>(d * c.g),
            static_cast<uint8_t>(d * c.b)};
}

///
/// Transition strategies define the profile a particular transition will take on. This is a good
/// default that just does a linear transition from start to finish
///
struct linear_transition_strategy
{
    ///
    /// Function that transition strategies need to define. If you think of the transition as a
    /// function that maps an X value (between 0 and 1) to a Y value (also between 0 and 1), the Y value
    /// represents the state of the transition at that X
    ///
    constexpr static double map(const double x)
    {
        return x;
    }
};

///
/// Does a C1 continuous transition, meaning the derivative is continuous
///
struct smooth_transition_strategy
{
    ///
    /// I'm using a cosine here, but it's range needs to be changed to [0, 1] - as well as doing a half
    /// period between [0, 1] (instead of [0, pi] like usual)
    ///
    constexpr static double map(const double x)
    {
        constexpr double VERTICAL_OFFSET = 1;
        constexpr double SCALE_FACTOR = 0.5;
        return 1.0 - (std::cos(x * M_PI) + VERTICAL_OFFSET) * SCALE_FACTOR;
    }
};

///
/// Does a sharp instant transition, goes from start to end immediately
///
struct instant_transition_strategy
{
    constexpr static double map(const double x)
    {
        return 1.0;
    }
};

///
/// Generate a transition with the provided number of steps and the given transition strategy, the
/// transition will NOT start at c1, but will be one step AFTER c1. It DOES end on c2 so the final state
/// is correct
///
template <uint8_t num_steps, typename transition_strategy = linear_transition_strategy>
constexpr std::array<Color, num_steps> generate_transition(const Color& c1, const Color& c2)
{
    constexpr double step_size = 1.0 / num_steps;

    const uint8_t r_diff_step = c2.r - c1.r;
    const uint8_t g_diff_step = c2.g - c1.g;
    const uint8_t b_diff_step = c2.b - c1.b;
    const Color c_step {r_diff_step, g_diff_step, b_diff_step};

    std::array<Color, num_steps> data;

    for (uint8_t i = 0; i < num_steps; ++i)
    {
        //
        // this_step will be between 0 and 1 for this step in the transition. That get's mapped using
        // the transition strategy to another value between 0 and 1 which gets applied to the color at
        // this step
        //
        const double this_step = (i + 1) * step_size;
        const double transition_step = transition_strategy::map(this_step);
        data[i] = c1 + transition_step * c_step;
    }

    return data;
}
