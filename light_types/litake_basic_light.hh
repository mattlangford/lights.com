#pragma once

#include "abstract_light.hh"

#include <atomic>
#include <algorithm>

namespace lights
{

class litake_basic_light final : public abstract_light
{
public: // types //////////////////////////////////////////////////////////////
    typedef std::shared_ptr<litake_basic_light> ptr;

    static constexpr size_t NUM_CHANNELS = 4;

public: // constructor ////////////////////////////////////////////////////////
    litake_basic_light(size_t start_address);

    virtual ~litake_basic_light() = default;

public: // methods ////////////////////////////////////////////////////////////
    // do a full atomic set of the color
    void set_color(uint8_t red, uint8_t green, uint8_t blue);

    // sets modifier params
    void set_off() override;
    // flash speed should be in [1, 127]
    void set_flash(uint8_t flash_speed);
    // brightness speed should be in [1, 127]
    void set_brightness(uint8_t brightness);

    // required to fit the interface
    inline size_t get_start_address() const override { return start_address_; }
    inline size_t get_end_address() const override { return start_address_ + NUM_CHANNELS - 1; }

    std::vector<dmx::dmx_helper::channel_t> get_channels() const override;

    // Set or get this light as a json object
    json::json get_json_light_state() const override;
    void set_json_light_state(const json::json& j) override;

private: // members ///////////////////////////////////////////////////////////
    size_t start_address_;

    // The state of the light (RGB) is kept as a single uint32_t so that atomic operations
    // can be done at once. The assumption is the mod value won't produce unwanted results

    std::atomic<uint8_t> mod_state_value_;
    std::atomic<uint32_t> light_state_;
};

}
