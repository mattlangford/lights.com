#include "config/universe.hh"
#include "light_control/light_universe_controller.hh"

#include "json/json.hh"
#include "resources/json_resource.hh"

namespace server_hooks
{

///
///
///
class universe_resource final : public resources::json_resource
{
public: ///////////////////////////////////////////////////////////////////////
    //
    // Construct with a config for a universe and a pointer to channels that the universe is configured for
    //
    universe_resource(const config::universe& universe, light_control::light_universe_controller& controller);

    inline const std::string& get_resource_identifier() const override
    {
        static std::string resource_name = "/universe_state";
        return resource_name;
    }

    json::json get_json_resource() override;

    bool handle_post_request(requests::POST post_request) override;

private: //////////////////////////////////////////////////////////////////////
    const config::universe universe_;

    light_control::light_universe_controller& controller_;
};
}
