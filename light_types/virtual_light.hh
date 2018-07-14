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
    //
    // Set the relative channel, 0 will be the channel at start_address
    //
    void set_channel(const uint8_t channel, const uint8_t level);

public: // methods ////////////////////////////////////////////////////////////
    inline size_t get_start_address() const override { return channels_.front().address; }
    inline size_t get_end_address() const override { return channels_.back().address; }

    void set_channels(std::vector<uint8_t> channels) override;

    inline std::vector<dmx::channel_t> get_channels() const override { return channels_; }
    inline const std::vector<std::string>& get_channel_names() const override { return channel_names_; }

    void set_off() override;

private: //////////////////////////////////////////////////////////////////////
    std::vector<dmx::channel_t> channels_;
    std::vector<std::string> channel_names_;
};
}
