#pragma once

#include "node.hh"
#include "config.hh"
#include <memory>
#include <vector>

namespace light {
class Universe : public runner::Node {
public:
    Universe(const config::UniverseNode& config = {});
    ~Universe() override = default;

    void add_light(config::UniverseNode::Light light);
    void set_max_channel(uint16_t channel);

    size_t input_count() const override;

    void callback(runner::Context& context) override;

    const std::vector<uint8_t>& channels() const;

private:
    std::vector<config::UniverseNode::Light> lights_;

    std::vector<uint8_t> channels_;
};

}