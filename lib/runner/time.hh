#pragma once

#include <chrono>

namespace runner {
using Time = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;

inline float to_seconds(Time in) {
    return std::chrono::duration_cast<std::chrono::duration<float>>(in.time_since_epoch()).count();
}

inline Time from_seconds(float in) {
    return Time{std::chrono::duration_cast<Duration>(std::chrono::duration<float>(in))};
}
}