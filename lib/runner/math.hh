#pragma once

#include "node.hh"
#include "port.hh"
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


class OpNode final : public Inputs<ValuePort, ValuePort, OptionPort>, public Outputs<ValuePort> {
public:
    enum Ops : uint8_t {
        ADD = 0,
        SUBTRACT,
        SATURATE_ADD,
        SATURATE_SUBTRACT,
        UNKNOWN
    };

    void callback() {
        if (!writeable<0>()) {
            return;
        }

        Value lhs = read<0>();
        Value rhs = read<1>();
        switch (read<2, Ops>()) {
        case ADD:
            write<0>(lhs + rhs);
            return;
        default:
            break;
        }

    }
};
}