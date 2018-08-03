#pragma once

#include "light_control/scene_helpers.hh"

namespace light_control
{

struct sunset final : public abstract_scene
{
    virtual ~sunset() = default;
    std::deque<schedule_entry> get_schedule(const config::universe& universe) override;
};

}
