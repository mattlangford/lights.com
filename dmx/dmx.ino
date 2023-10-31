#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"

#include <MIDIUSB.h>

#include <map>
#include <set>
#include <vector>
#include <string>

DMXController* controller;

class EffectMap {
public:
    template <typename Effect, typename... Args>
    Effect* add_effect(const std::string& name, Args&&...args) {
        Effect* ptr = new Effect(args...);
        effects_[name].push_back(ptr);
        return ptr;
    }

    std::vector<std::string> names(const std::string& light_names) const {
        std::vector<std::string> out;
        out.reserve(effects_.size());
        for (const auto& it : effects_) {
            out.push_back(it.first);
        }
        return out;
    }

private:
    std::map<std::string, std::vector<Effect*>> effects_;
};

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

    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    for (size_t l = 0; l < Universe::NUM_KITCHEN; ++l) {
        const CosBlend::Config config {
            .depth = 2,
            .min_freq = 0.01,
            .max_freq = 1.0,
        };
        KitchenLight& light = *universe->kitchen[l];
        light.brightness.set_value(255);
        light.rgb.add_effect<CosBlend>(config).set_values_rgb(
            {.min=190, .max=255}, {.min=10, .max=50}, {.min=0, .max=0});
    }

    {
        const CosBlend::Config config {
            .depth = 2,
            .min_freq = 0.1,
            .max_freq = 10,
        };
        WashBarLight112& light = *universe->bar1;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            light.rgb(i).add_effect<CosBlend>(config).set_values_rgb(
                {.min=190, .max=255}, {.min=10, .max=50}, {.min=0, .max=0});
            light.white(i).add_effect<LinearFade>().set_values({.min=0, .max=20});
        }
    }
    {
        WashBarLight112& light = *universe->bar2;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            if (i % 7 == 0) {
                light.rgb(i).set_value_rgb(175, 20, 0);
            }
        }
    }
}

uint32_t now = millis();

void loop() {
    controller->write_frame();

    if (millis() - now > 1000) { //static_cast<uint32_t>(random(10000, 30000))) {
        now = millis();
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            //universe->bar1->whites[i]->effect(0)->trigger(now + 100 * i);
        }
    }

}

