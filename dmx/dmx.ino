#include <MIDIUSB.h>

#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"
#include "cli.hh"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include <vector>

DMXController* controller;
Interface interface;
EffectMap effects;

struct Universe {
    static constexpr size_t NUM_BARS = 3;
    WashBarLight112* bar[NUM_BARS];

    Universe(DMXController& controller) {
        bar[0] = new WashBarLight112(1, controller);
        bar[1] = new WashBarLight112(113, controller);
        bar[2] = new WashBarLight112(225, controller);
    }
};
Universe* universe;

const char BASE_CONFIG[] =  R"=====(
{"background":{"values":{"red":{"min":0,"max":0},"green":{"min":0,"max":0},"blue":{"min":0,"max":0}}},"flash":{ "config":{"rise_dt_ms":10,"hold_dt_ms":0,"fall_dt_ms":300},"values":{"min":0,"max":255}}}
)=====";

void list(const std::string&) {
    serializeJson(effects.get_json(), Serial);
    Serial.println();
}
void trigger(const std::string& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        effect->trigger(millis());
    } else {
        Serial.print("Unable to find effect named '");
        Serial.print(name.c_str());
        Serial.println("'");
    }
}
void clear(const std::string& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        effect->clear(millis());
    } else {
        Serial.print("Unable to find effect named '");
        Serial.print(name.c_str());
        Serial.println("'");
    }
}
void set(const std::string& json) {
    DynamicJsonDocument doc(1024);
    const DeserializationError err = deserializeJson(doc, json);
    if (err.code() == DeserializationError::Ok) {
        effects.set_json(doc.as<JsonObject>());
        Serial.println("Updated config!");
    } else {
        Serial.print("Unable to parse json input: ");
        Serial.println(err.c_str());
    }
}
void help(const std::string&) {
    interface.help();
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    // auto& background = effects.add_effect<CompositeEffect<RgbEffect<CosBlend>>>("background");
    auto& pulse = effects.add_effect<SweepingPulse>("pulse");

    for (size_t l = 0; l < Universe::NUM_BARS; ++l) {
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            // auto& rgb = background.add();
            // universe->bar[l]->red(i).add_effect(rgb.red());
            // universe->bar[l]->green(i).add_effect(rgb.green());
            // universe->bar[l]->blue(i).add_effect(rgb.blue());

            universe->bar[l]->red(i).add_effect(&pulse.add());
        }
    }
    effects.effect("pulse")->trigger(millis());

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, BASE_CONFIG);
    effects.set_json(doc.as<JsonObject>());

    interface.add_handler("list", "show current configuration", &list);
    interface.add_handler("trigger", "trigger the specified effect", &trigger);
    interface.add_handler("clear", "clear the specified effect", &clear);
    interface.add_handler("set", "sets current config from a JSON string", &set);
    interface.add_handler("help", "show this information", &help);
}

uint32_t last = 0;
void loop() {
    interface.handle_serial();

    controller->write_frame();

    uint32_t now = millis();
    if (now - last > 20000) {
        effects.effect("pulse")->trigger(now);
        last = now;
    }
}

