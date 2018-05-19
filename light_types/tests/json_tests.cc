#include "light_types/litake_basic_light.hh"

#include <assert.h>

int main()
{
    std::cout << "Running test!\n";

    lights::litake_basic_light test_light_0(0);
    auto state = test_light_0.get_json_light_state();
    std::cout << state.get_value_as_string() << "\n";

    json::json new_state;
    new_state.set_map();
    new_state["red"] = (double)24;
    new_state["green"] = (double)155;
    test_light_0.set_json_light_state(new_state);

    state = test_light_0.get_json_light_state();
    assert(state["channel_state"]["red"]["level"].get<double>() == 24);
    assert(state["channel_state"]["green"]["level"].get<double>() == 155);
    assert(state["channel_state"]["blue"]["level"].get<double>() == 0);

    std::cout << "Success!\n";
}
