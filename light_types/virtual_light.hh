#pragma once

#include "light_types/abstract_light.hh"
#include <cstddef>

namespace lights
{

class virtual_light final : public abstract_light
{
public: // constructor/destructor /////////////////////////////////////////////
    virtual_light(const uint8_t start_address, const uint8_t num_channels);
    virtual ~virtual_light() = default;

public: ///////////////////////////////////////////////////////////////////////
    void set_channel(const uint8_t channel, const uint8_t level);

public: // methods ////////////////////////////////////////////////////////////
    inline size_t get_start_address() const override { return channels.front().address; }
    inline size_t get_end_address() const override { return channels.back().address; }

    inline std::vector<dmx::dmx_helper::channel_t> get_channels() const override { return channels; }

    void set_off() override;

    json::json get_json_light_state() const override;
    void set_json_light_state(const json::json& j) override;

private: //////////////////////////////////////////////////////////////////////
    std::vector<dmx::dmx_helper::channel_t> channels;
};
}
