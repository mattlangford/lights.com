#pragma once

#include "node.hh"

namespace runner {
class Adder final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override  {
        context.output(0, context.input(0) + context.input(1));
    }
};

class Subtractor final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override {
        context.output(0, context.input(0) - context.input(1));
    }
};

class Multiplier final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override {
        context.output(0, context.input(0) * context.input(1));
    }
};

class Divider final : public Node {
public:
    size_t input_count() const override { return 2; }
    size_t output_count() const override { return 1; }
    void callback(Context& context) override {
        context.output(0, context.input(0) / context.input(1));
    }
};
}