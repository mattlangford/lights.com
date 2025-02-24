#pragma once

#include "config.hh"
#include "runner.hh"

namespace runner {
std::shared_ptr<Node> build(const config::Node& node_config);

Runner build(const config::Runner& runner_config);
}