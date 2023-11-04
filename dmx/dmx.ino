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
    static constexpr size_t NUM_KITCHEN = 4;
    KitchenLight* kitchen[NUM_KITCHEN];

    static constexpr size_t NUM_BAR = 1;
    WashBarLight112* bar1;
    WashBarLight112* bar2;

    Universe(DMXController& controller) {
        kitchen[0] = new KitchenLight(1, controller);
        kitchen[1] = new KitchenLight(8, controller);
        kitchen[2] = new KitchenLight(15, controller);
        kitchen[3] = new KitchenLight(22, controller);

        bar1 = new WashBarLight112(40, controller);
        bar2 = new WashBarLight112(152, controller);
    }
};
Universe* universe;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    const CosBlendConfig config {
        .depth = 2,
        .min_freq = 0.01,
        .max_freq = 1.0,
    };
    auto& background = effects.add_effect<CompositeEffect<RgbEffect<CosBlend>>>("Background");

    for (size_t l = 0; l < Universe::NUM_KITCHEN; ++l) {
        KitchenLight& light = *universe->kitchen[l];
        light.brightness.set_value(255);

        auto& rgb = background.add();
        light.red.add_effect(rgb.red());
        light.green.add_effect(rgb.green());
        light.blue.add_effect(rgb.blue());
    }

    {
        WashBarLight112& light = *universe->bar1;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            auto& rgb = background.add();
            light.red(i).add_effect(rgb.red());
            light.green(i).add_effect(rgb.green());
            light.blue(i).add_effect(rgb.blue());
        }
    }
    {
        WashBarLight112& light = *universe->bar2;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            if (i % 7 == 0) {
                light.red(i).set_value(175);
                light.green(i).set_value(22);
            }
        }
    }
}

void loop() {
    const auto str = Serial.readString().trim();
    if (str.length() == 0) {
    } else if (str == "list") {
        serializeJson(effects.get_json(), Serial);
        Serial.println();
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
        Serial.println("'");
    }

    controller->write_frame();
}

