#pragma once

#include "time.hh"
#include <vector>

namespace runner {
class Context {
public:
    float input(size_t port) const { return values_.at(inputs_.at(port)); }
    void output(size_t port, float value) const { values_.at(outputs_.at(port)) = value; }

    Time now() const { return now_; }
    Duration dt() const { return dt_; };

private:
    friend class Runner;
    Context(
        Time now,
        Duration dt,
        std::vector<float>& values,
        const std::vector<size_t>& in,
        const std::vector<size_t>& out
    ) : now_(now), dt_(dt), values_(values), inputs_(in), outputs_(out) { }

    const Time now_;
    const Duration dt_;
    std::vector<float>& values_;
    const std::vector<size_t>& inputs_;
    const std::vector<size_t>& outputs_;
};
}