#pragma once

#include "light_control/scene_helpers.hh"

namespace light_control
{

struct submarine : public abstract_scene
{
    virtual ~submarine() = default;
    std::deque<schedule_entry> get_schedule(const config::universe& universe) override;
};

}
