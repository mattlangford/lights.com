#pragma once

#include "node.hh"
#include "config.hh"

namespace runner {
class Adder final : public Node {
public:
    explicit Adder(const config::AdderNode& config={});
    size_t input_count() const override { return config_.inputs; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override;
private:
    const config::AdderNode config_;
};

class Subtractor final : public Node {
public:
    explicit Subtractor(const config::SubtractorNode& config={});
    size_t input_count() const override { return config_.inputs; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override;
private:
    const config::SubtractorNode config_;
};

class Multiplier final : public Node {
public:
    explicit Multiplier(const config::MultiplierNode& config={});
    size_t input_count() const override { return config_.inputs; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override;
private:
    const config::MultiplierNode config_;
};

class Divider final : public Node {
public:
    explicit Divider(const config::DividerNode& config={});
    size_t input_count() const override { return config_.inputs; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override;
private:
    const config::DividerNode config_;
};
}