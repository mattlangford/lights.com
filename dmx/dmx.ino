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
    WashBarLight112* bar;

    Universe(DMXController& controller) {
        bar = new WashBarLight112(1, controller);
    }
};
Universe* universe;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    auto& background = effects.add_effect<CompositeEffect<RgbEffect<CosBlend>>>("Background");

    for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
        auto& rgb = background.add();
        universe->bar->red(i).add_effect(rgb.red());
        universe->bar->green(i).add_effect(rgb.green());
        universe->bar->blue(i).add_effect(rgb.blue());
    }
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

