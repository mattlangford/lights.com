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
    LitakeLight l0;
    MissyeeLight l1;
    BetopperMover9Light l2;

    Universe(DMXController& controller):
        l0(1, controller),
        l1(10, controller),
        l2(20, controller) {
    }
};
Universe* universe;

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

    auto* x = &effects.add_effect<CosBlend>("x");
    auto* y = &effects.add_effect<CosBlend>("y");
    auto* r = &effects.add_effect<CosBlend>("r");

    x->set_config(CosBlendConfig{.freq={0.5}, .phase0={0.0}});
    y->set_config(CosBlendConfig{.freq={0.1}, .phase0={M_PI / 2.0}});
    r->set_config(CosBlendConfig{.freq={1.0, 0.25}, .phase0={0.2, 0.0}});

    universe->l0.red.add_effect(r);
    universe->l1.red.add_effect(r);
    universe->l2.red.add_effect(r);

    universe->l2.x.add_effect(x);
    universe->l2.y.add_effect(y);
}


void loop() {
    midi.read();
    audio.read();
    interface.handle_serial();
    controller->write_frame();
}


