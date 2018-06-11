#include "light_control/light_universe_controller.hh"

#include "json/json.hh"
#include "resources/json_resource.hh"

// TODO: Make this a weak_ptr
using universe_map = std::map<std::string, lights::abstract_light::ptr>;

class universe_resource_builder;

///
///
///
class universe_resource final : public resources::json_resource
{
public: ///////////////////////////////////////////////////////////////////////
    inline const std::string& get_resource_identifier() const override
    {
        static std::string resource_name = "/universe_state";
        return resource_name;
    }

    json::json get_json_resource() override;

    void handle_universe_update(const json::json& update);

private: //////////////////////////////////////////////////////////////////////
    universe_map light_map;

    friend class universe_resource_builder;
};

///
///
///
class universe_resource_builder
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    universe_resource_builder(lights::light_universe_controller& controller);

public: ///////////////////////////////////////////////////////////////////////

    ///
    /// Registers the light with this resource builder AND with the controller
    ///
    void add_light_to_universe(lights::abstract_light::ptr light);

    ///
    /// Finish building the universe, this resource can be used to service HTTP Requests
    ///
    universe_resource finalize();

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Controller we'll add lights to
    ///
    lights::light_universe_controller& controller;

    ///
    /// The map that we're building, as lights get added, they get identifiers added and put here
    ///
    universe_map building_map;
};
