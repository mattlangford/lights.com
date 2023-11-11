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
        Serial.println(err);
    }
}
void help(const String&) {
    interface.help();
}

class AudioMeter : public EffectBase {
public:
    using Effect = RgbEffect<AudioLevel>;
public:
    AudioMeter(size_t count, uint8_t port) : effects_(count) {
        size_t red_index = 0.2 * count;
        size_t yellow_index = 0.5 * count;

        for (size_t i = 0; i < count; ++i) {
            AudioLevelConfig config;
            config.threshold = 0.8 * std::pow(10, -static_cast<float>(i) / (count - 1));
            config.port = port;

            effects_[i].set_config(config);

            if (i < red_index) {
                effects_[i].set_max_values(128, 0, 0);
            } else if (i < yellow_index) {
                effects_[i].set_max_values(128, 128, 0);
            } else {
                effects_[i].set_max_values(0, 128, 0);
            }
        }
    }
    ~AudioMeter() override = default;

    void set_config_json(const JsonObject& json) {}
    void get_config_json(JsonObject& json) const {}
    void set_values_json(const JsonObject& json) {}
    void get_values_json(JsonObject& json) const {}

    String type() const { return "AudioMeter"; }

    virtual void trigger(uint32_t now_ms) {}
    virtual void clear(uint32_t now_ms) {}

    // Index 0 is the top of the meter (red)
    Effect* effect(size_t i) { return i < effects_.size() ? &effects_[i] : nullptr; }

private:
    std::vector<Effect> effects_;
};

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    interface.add_handler("list", "show current configuration", &list);
    interface.add_handler("trigger", "trigger the specified effect", &trigger);
    interface.add_handler("clear", "clear the specified effect", &clear);
    interface.add_handler("set", "sets current config from a JSON string", &set);
    interface.add_handler("help", "show this information", &help);

    controller = new DMXController(41, 40);

    audio.setup();
    midi.setup();
    universe.setup(*controller);

    auto& meter_l = effects.add_effect<AudioMeter>("audio_l", WashBarLight112::NUM_LIGHTS, 0);
    auto& meter_r = effects.add_effect<AudioMeter>("audio_r", WashBarLight112::NUM_LIGHTS, 1);
    for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
        auto& effect_l = *meter_l.effect(WashBarLight112::NUM_LIGHTS - i - 1);
        auto& effect_r = *meter_r.effect(WashBarLight112::NUM_LIGHTS - i - 1);

        auto& bar_l = *universe.bar[0];
        bar_l.red(i).add_effect(effect_l.red());
        bar_l.green(i).add_effect(effect_l.green());
        bar_l.blue(i).add_effect(effect_l.blue());

        auto& bar_r = *universe.bar[1];
        bar_r.red(i).add_effect(effect_r.red());
        bar_r.green(i).add_effect(effect_r.green());
        bar_r.blue(i).add_effect(effect_r.blue());
    }
}


void loop() {
    midi.read();
    audio.read();
    interface.handle_serial();
    controller->write_frame();
}


