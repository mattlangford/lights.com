#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"

#include <MIDIUSB.h>

DMXController* controller;

struct Universe {
    static constexpr size_t NUM_KITCHEN = 4;
    KitchenLight* kitchen[NUM_KITCHEN];

    static constexpr size_t NUM_BAR = 2;
    WashBarLight112* bar[NUM_BAR];

    Universe(DMXController& controller) {
        kitchen[0] = new KitchenLight(1, controller);
        kitchen[1] = new KitchenLight(8, controller);
        kitchen[2] = new KitchenLight(15, controller);
        kitchen[3] = new KitchenLight(22, controller);

        bar[0] = new WashBarLight112(40, controller);
        bar[1] = new WashBarLight112(152, controller);
    }
};
Universe* universe;

void note_on(byte channel, byte note, byte velocity) {
    Serial.print("Note ch=");
    Serial.print((int)channel);
    Serial.print(" note=");
    Serial.print((int)note);
    Serial.print(" velocity=");
    Serial.println((int)velocity);
}

void note_off(byte channel, byte note, byte velocity) {
}

void setup() {
    Serial.begin(115200);

    controller = new DMXController(41, 40);
    universe = new Universe(*controller);

    const CosBlend::Config config {
        .depth = 3,
        .min_freq = 0.1,
        .max_freq = 10,
    };
    const ValueConfig r{.min=175, .max=255};
    const ValueConfig g{.min=120, .max=200};
    const ValueConfig b{.min=0, .max=30};

    for (size_t l = 0; l < Universe::NUM_KITCHEN; ++l) {
        KitchenLight& light = *universe->kitchen[l];
        light.brightness.set_value(255);
        light.rgb.add_effect<CosBlend>(config).set_values_rgb(r, g, b);
    }
    for (size_t l = 0; l < Universe::NUM_BAR; ++l) {
        WashBarLight112& light = *universe->bar[l];
        for (size_t i = 0; i < WashBarLight112::NUM_LIGHTS; ++i) {
            light.rgb[i].add_effect<CosBlend>(config).set_values_rgb(r, g, b);
        }
    }

    usbMIDI.setHandleNoteOn(note_on);
    usbMIDI.setHandleNoteOff(note_off);
}

void loop() {
    usbMIDI.read();
    controller->write_frame();
}

