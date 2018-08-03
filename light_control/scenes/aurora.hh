#pragma once

#include "light_control/scene_helpers.hh"

namespace light_control
{

struct aurora final : public abstract_scene
{
    virtual ~aurora() = default;
    std::deque<schedule_entry> get_schedule(const config::universe& universe) override;
};

}
