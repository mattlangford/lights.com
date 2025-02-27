#pragma once

#include "context.hh"
#include <string>

namespace runner {
class Node {
public:
    virtual ~Node() = default;

    virtual size_t input_count() const { return 0; }
    virtual size_t output_count() const { return 0; }

    virtual void callback(Context& context) = 0;
};
}