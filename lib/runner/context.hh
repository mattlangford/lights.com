#pragma once
#include <vector>

class Context {
public:
    float input(size_t port) const { return values_.at(inputs_.at(port)); }
    void output(size_t port, float value) const { values_.at(outputs_.at(port)) = value; }

    inline uint32_t now() const { return now_; }

private:
    friend class Runner;
    Context(
        uint32_t now,
        std::vector<float>& values,
        const std::vector<size_t>& in,
        const std::vector<size_t>& out
    ) : now_(now), values_(values), inputs_(in), outputs_(out) { }

    const uint32_t now_;
    std::vector<float>& values_;
    const std::vector<size_t>& inputs_;
    const std::vector<size_t>& outputs_;
};