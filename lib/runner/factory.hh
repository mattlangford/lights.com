#pragma once

#include "config.hh"

namespace runner {
std::shared_ptr<Node> build(const config::Node& node_config);
}