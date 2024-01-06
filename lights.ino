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
    static constexpr size_t NUM_BARS = 3;
    WashBarLight112* bar[NUM_BARS];

    void setup(DMXController& controller) {
        bar[0] = new WashBarLight112(1, controller);
        bar[1] = new WashBarLight112(113, controller);
        bar[2] = new WashBarLight112(225, controller);
    }
};
Universe universe;

void list(const String&) {
    serializeJson(effects.get_json(), Serial);
    Serial.println();
}
void trigger(const String& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        Serial.print("Triggering ");
        Serial.println(name);
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
void set(const String& json) {
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

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    interface.add_handler("list", "show current configuration", &list);
    interface.add_handler("trigger", "trigger the specified effect", &trigger);
    interface.add_handler("clear", "clear the specified effect", &clear);
    interface.add_handler("set", "sets current config from a JSON string", &set);

    controller = new DMXController(41, 40);

    audio.setup();
    midi.setup();
    universe.setup(*controller);

    auto& freq = effects.add_effect<CompositeEffect<RgbEffect<AudioFrequencyValues>>>("freq");

    auto& meter_l = effects.add_effect<AudioMeter>("audio_l", WashBarLight112::NUM_LIGHTS, 0);
    auto& meter_r = effects.add_effect<AudioMeter>("audio_r", WashBarLight112::NUM_LIGHTS, 1);

    for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
        auto& effect_l = meter_l.rgb(WashBarLight112::NUM_LIGHTS - i - 1);
        auto& effect_r = meter_r.rgb(WashBarLight112::NUM_LIGHTS - i - 1);

        auto& bar_l = *universe.bar[0];
        bar_l.red(i).add_effect(&effect_l.red());
        bar_l.green(i).add_effect(&effect_l.green());
        bar_l.blue(i).add_effect(&effect_l.blue());

        auto& bar_r = *universe.bar[1];
        bar_r.red(i).add_effect(&effect_r.red());
        bar_r.green(i).add_effect(&effect_r.green());
        bar_r.blue(i).add_effect(&effect_r.blue());

        auto& this_freq = freq.add();
        AudioFrequencyLevelConfig freq_config;
        constexpr size_t SPACING = 1;
        freq_config.min_bin = SPACING * i;
        freq_config.max_bin = SPACING * (i + 1);
        this_freq.red().set_values(10, 255);
        this_freq.green().set_values(3, 75);
        this_freq.blue().set_values(0, 0);
        this_freq.set_config(freq_config);
        auto& bar_freq = *universe.bar[2];
        bar_freq.red(i).add_effect(&this_freq.red());
        bar_freq.green(i).add_effect(&this_freq.green());
        bar_freq.blue(i).add_effect(&this_freq.blue());
    }
}


void loop() {
    midi.read();
    audio.read();
    interface.handle_serial();
    controller->write_frame();
}


