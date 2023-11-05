#include <MIDIUSB.h>

#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include <vector>

DMXController* controller;
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
    {"Background":{
      "config":{
        "red":{"depth":3,"min_freq":1,"max_freq":10,"passthrough":0},
        "green":{"depth":3,"min_freq":1,"max_freq":10,"passthrough":0},
        "blue":{"depth":3,"min_freq":1,"max_freq":10,"passthrough":0}},
      "values":{
        "red":{"min":1,"max":50},
        "green":{"min":0,"max":0},
        "blue":{"min":1,"max":100}}
    }}"
)=====";

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    auto& background = effects.add_effect<CompositeEffect<RgbEffect<CosBlend>>>("Background");

    for (size_t l = 0; l < Universe::NUM_BARS; ++l) {
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            auto& rgb = background.add();
            universe->bar[l]->red(i).add_effect(rgb.red());
            universe->bar[l]->green(i).add_effect(rgb.green());
            universe->bar[l]->blue(i).add_effect(rgb.blue());
        }
    }

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, BASE_CONFIG);
    effects.set_json(doc.as<JsonObject>());
}

void loop() {
    const auto str = Serial.readString().trim();
    if (str.length() == 0) {
    } else if (str == "list") {
        serializeJson(effects.get_json(), Serial);
        Serial.println();
    } else if (str == "help") {
        Serial.println("Commands:");
        Serial.println(" - 'list' to show current configuration");
        Serial.println(" - 'set JSON_STR' to set config");
        Serial.println(" - 'help' to show this help");
    } else if (str.startsWith("set ")) {
        DynamicJsonDocument doc(1024);
        const DeserializationError err = deserializeJson(doc, str.substring(4));
        if (err.code() == DeserializationError::Ok) {
            effects.set_json(doc.as<JsonObject>());
            Serial.println("Updated config!");
        } else {
            Serial.print("Unable to parse json input: ");
            Serial.println(err.c_str());
        }
    } else {
        Serial.print("Unknown command: '");
        Serial.print(str);
        Serial.println("' Send 'help' for commands.");
    }

    controller->write_frame();
}

