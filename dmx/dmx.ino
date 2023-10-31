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
    ~EffectMap() {
        for (auto& it : effects_) {
            for (auto& effect : it.second) {
                delete effect;
            }
        }
    }

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

    const std::vector<Configurable*>& effects(const std::string& name) const {
        static std::vector<Configurable*> empty;
        auto it = effects_.find(name);
        return it == effects_.end() ? empty : it->second;
    }

private:
    std::map<std::string, std::vector<Configurable*>> effects_;
};
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

        auto rgb = effects.add_effect<RgbEffect<CosBlend>>("Background", config);
        light.red.add_effect(rgb->red());
        light.green.add_effect(rgb->green());
        light.blue.add_effect(rgb->blue());
        rgb->set_values_rgb({.min=190, .max=255}, {.min=10, .max=50}, {.min=0, .max=0});
    }

    {
        const CosBlend::Config config {
            .depth = 2,
            .min_freq = 0.1,
            .max_freq = 10,
        };
        WashBarLight112& light = *universe->bar1;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            auto rgb = effects.add_effect<RgbEffect<CosBlend>>("Background", config);
            rgb->set_values_rgb({.min=190, .max=255}, {.min=10, .max=50}, {.min=0, .max=0});
            light.red(i).add_effect(rgb->red());
            light.green(i).add_effect(rgb->green());
            light.blue(i).add_effect(rgb->blue());
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
    controller->write_frame();
}

