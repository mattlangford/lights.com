#pragma once

#include "config/universe.hh"
#include "light_control/scheduling.hh"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace light_control
{

struct abstract_scene
{
    ///
    /// Get the schedule of this scene
    ///
    virtual std::deque<schedule_entry> get_schedule(const config::universe& universe) = 0;
};

using abstract_scene_ptr = std::shared_ptr<abstract_scene>;

//
// ############################################################################
//

class scene_helper
{
public:
    ///
    /// Returns singleton instance of the scene_helper
    ///
    static scene_helper& get_instance();

public:
    inline void register_scene(const std::string& name, abstract_scene_ptr scene)
    {
        registered_[name] = std::move(scene);
    }

    inline abstract_scene_ptr get_scene(const std::string& name) const
    {
        auto found = registered_.find(name);
        if (found == registered_.cend())
        {
            return nullptr;
        }

        return found->second;
    }

    inline std::vector<std::string> get_registered_scenes() const
    {
        std::vector<std::string> keys;
        std::transform(
                registered_.begin(),
                registered_.end(),
                std::back_inserter(keys),
                [](const decltype(registered_)::value_type &pair){return pair.first;});
        return keys;
    }

private:
    std::map<std::string, abstract_scene_ptr> registered_;
};

#define REGISTER_SCENE(name, scene)                                \
    struct name##_registrar                                        \
    {                                                              \
        name##_registrar()                                         \
        {                                                          \
            light_control::scene_helper::get_instance()            \
                .register_scene(#name, std::make_shared<scene>()); \
        }                                                          \
    };                                                             \
    static name##_registrar name##_registrar_impl{};
}
