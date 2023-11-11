
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

void list(const std::string&) {
    serializeJson(effects.get_json(), Serial);
    Serial.println();
}
void trigger(const std::string& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        Serial.print("Triggering ");
        Serial.println(name.c_str());
        effect->trigger(millis());
    }
}
void clear(const std::string& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        Serial.print("Clearing ");
        Serial.println(name.c_str());
        effect->clear(millis());
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

LinearPulse *pulse;
void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    midi.setup();
    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    // auto& background = effects.add_effect<CompositeEffect<RgbEffect<CosBlend>>>("background");
    auto& midi_red = effects.add_effect<MidiTrigger<LinearFade>>("midi_red");
    pulse = &effects.add_effect<LinearPulse>("pulse");

    for (size_t l = 0; l < Universe::NUM_BARS; ++l) {
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            // auto& rgb = background.add();
            // universe->bar[l]->red(i).add_effect(rgb.red());
            // universe->bar[l]->green(i).add_effect(rgb.green());
            // universe->bar[l]->blue(i).add_effect(rgb.blue());

            universe->bar[l]->red(i).add_effect(midi_red.effect());
            universe->bar[l]->red(i).add_effect(pulse);
        }
    }

    midi_red.set_note(72);
    pulse->trigger(millis());

    interface.add_handler("list", "show current configuration", &list);
    interface.add_handler("trigger", "trigger the specified effect", &trigger);
    interface.add_handler("clear", "clear the specified effect", &clear);
    interface.add_handler("set", "sets current config from a JSON string", &set);
    interface.add_handler("help", "show this information", &help);
}

elapsedMillis pulse_t;
void loop() {
    midi.read();
    interface.handle_serial();
    controller->write_frame();

    if (pulse_t > 3000) {
        pulse->trigger(millis());
        pulse_t = 0;
    }
}


