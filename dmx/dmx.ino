#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"

#include <MIDIUSB.h>

DMXController* controller;

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
        const ValueConfig r{.min=190, .max=230};
        const ValueConfig g{.min=75, .max=90};
        const ValueConfig b{.min=0, .max=0};
        const CosBlend::Config config {
            .depth = 2,
            .min_freq = 0.01,
            .max_freq = 1.0,
        };
        KitchenLight& light = *universe->kitchen[l];
        light.brightness.set_value(255);
        light.rgb.add_effect<CosBlend>(config).set_values_rgb(r, g, b);
    }

    {
        const ValueConfig r{.min=150, .max=255};
        const ValueConfig g{.min=50, .max=200};
        const ValueConfig b{.min=0, .max=30};
        const CosBlend::Config config {
            .depth = 2,
            .min_freq = 0.1,
            .max_freq = 10,
        };
        WashBarLight112& light = *universe->bar1;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            light.rgb[i].add_effect<CosBlend>(config).set_values_rgb(r, g, b);
        }
    }
    {
        const ValueConfig w{.min=10, .max=200};
        const CosBlend::Config config {
            .depth = 2,
            .min_freq = 0.1,
            .max_freq = 10,
        };
        WashBarLight112& light = *universe->bar2;
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            light.whites[i]->add_effect(new CosBlend(config))->set_values(w);
        }
    }
}

static float f = 100;
void loop() {
    controller->write_frame();

    WashBarLight112& light = *universe->bar2;
    for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
        light.whites[i]->set_value(fmod(f, 255));
    }
    f += 0.1;
}

