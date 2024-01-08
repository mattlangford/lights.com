#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"
#include "cli.hh"

#include <map>
#include <set>
#include <vector>
#include <memory>
#include <vector>

DMXController* controller;
Interface interface;
EffectMap effects;

struct Universe {
    LitakeLight litake[2];
    MissyeeLight missyee[4];
    BetopperMover9Light mover;
    WashBarLight112 bar[3];

    Universe(DMXController& controller):
        litake{{1, controller}, {5, controller}},
        missyee{{10, controller}, {17, controller}, {24, controller}, {31, controller}},
        mover{40, controller},
        bar{{50, controller}, {162, controller}, {274, controller}} {
        
        // controller.set_max_channel(512);

        litake[0].red.set_value(255);
        litake[1].red.set_value(255);
        // missyee[0].red.set_value(255);
        // missyee[1].red.set_value(255);
        // missyee[2].red.set_value(255);
        // missyee[3].red.set_value(255);
        // mover.red.set_value(255);
        // bar[0].set_color(255, 0, 0);
        // bar[1].set_color(255, 0, 0);
        // bar[2].set_color(255, 0, 0);
    }
};
Universe* universe;

void list(const String& name) {
    serializeJson(effects.get_json(name), Serial);
    Serial.println();
}
void trigger(const String& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        Serial.print("Triggering '");
        Serial.print(name);
        Serial.println('\'');
        effect->trigger(millis());
    }
}
void clear(const String& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        Serial.print("Clearing ");
        Serial.println(name);
        effect->clear(millis());
    }
}
void set(const String& name_and_json) {
    const size_t space = name_and_json.indexOf(' ');
    String name = name_and_json.substring(0, space);
    String json = name_and_json.substring(space + 1);

    DynamicJsonDocument doc(1024);
    const DeserializationError err = deserializeJson(doc, json);
    if (err.code() == DeserializationError::Ok) {
        effects.set_json(name, doc.as<JsonObject>());
        Serial.println("Updated config!");
    } else {
        Serial.print("Unable to parse json input: ");
        Serial.println(err.c_str());
    }
}
void values(const String& json) {
    auto pad100 = [](size_t in){
        if (in < 100) Serial.print(' ');
        if (in < 10) Serial.print(' ');
        Serial.print(in);
    };

    size_t max_channel = controller->max_channel();
    for (size_t ch = 0; ch < max_channel; ++ch) {
        if (ch % 10 == 0) {
            if (ch > 0) Serial.println();
            pad100(ch);
            Serial.print(" : ");
        }

        pad100(controller->channel(ch).get_value(millis()));
        Serial.print(", ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    interface.add_handler("list", "show current configuration", &list);
    interface.add_handler("trigger", "trigger the specified effect", &trigger);
    interface.add_handler("clear", "clear the specified effect", &clear);
    interface.add_handler("set", "sets current config from a JSON string", &set);
    interface.add_handler("values", "gets the current values of all channels", &values);

    controller = new DMXController(41, 40);

    audio.setup();
    midi.setup();

    universe = new Universe(*controller);

    // This is ordered around the room 
    auto& pulse = effects.add_effect<SweepingPulse>("pulse");
    universe->missyee[0].blue.add_effect(&pulse.add());
    universe->litake[0].blue.add_effect(&pulse.add());
    universe->litake[1].blue.add_effect(&pulse.add());
    universe->mover.blue.add_effect(&pulse.add());
    universe->missyee[1].blue.add_effect(&pulse.add());
    for (size_t l = 0; l < WashBarLight112::NUM_LIGHTS; ++l) {
        universe->bar[0].blue(l).add_effect(&pulse.add());
    }
    universe->missyee[2].blue.add_effect(&pulse.add());
    universe->missyee[3].blue.add_effect(&pulse.add());
    for (size_t l = 0; l < WashBarLight112::NUM_LIGHTS; ++l) {
        universe->bar[1].blue(l).add_effect(&pulse.add());
    }
    for (size_t l = 0; l < WashBarLight112::NUM_LIGHTS; ++l) {
        universe->bar[2].blue(l).add_effect(&pulse.add());
    }

    // auto* pan = &effects.add_effect<CosBlend>("pan");
    // pan->set_config(CosBlendConfig{.freq={0.3}});
    // pan->set_values(170, 250); // sweep the room!
    universe->mover.y.set_value(20);
    // universe->mover.x.add_effect(pan);
}

void loop() {
    midi.read();
    audio.read();
    interface.handle_serial();
    controller->write_frame();
}


