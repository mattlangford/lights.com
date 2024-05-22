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
    LitakeLight litake[2];
    MissyeeLight missyee[4];
    BetopperMover9Light mover;
    WashBarLight112 bar[3];

    Universe(DMXController& controller):
        litake{{5, controller}, {1, controller}},
        missyee{{10, controller}, {17, controller}, {31, controller}, {24, controller}},
        mover{40, controller},
        bar{{162, controller}, {274, controller}, {50, controller}} {

        controller.set_max_channel(512);

        // uint8_t red = 255;
        // uint8_t green = 30;

        // litake[0].red.set_value(red);
        // litake[1].blue.set_value(red);
        // missyee[0].red.set_value(red);
        // missyee[1].blue.set_value(red);
        // missyee[2].green.set_value(red);
        // missyee[3].red.set_value(red);
        // missyee[3].green.set_value(red);
        // missyee[3].blue.set_value(red);
        // mover.red.set_value(red);
        // mover.green.set_value(green);
        // bar[0].set_color(red, 0, 0);
        // bar[1].set_color(0, green, 0);
        // bar[2].set_color(red, green, 0);
    }
};
Universe* universe;

void list(const String& name) {
    serializeJson(effects.get_json(name), Serial);
    Serial.println();
}
void trigger(const String& name) {
    EffectBase* effect = effects.effect(name);
    if (effect != nullptr) {
        Serial.print("Triggering '");
        Serial.print(name);
        Serial.println('\'');
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
void set(const String& name_and_json) {
    const size_t space = name_and_json.indexOf(' ');
    String name = name_and_json.substring(0, space);
    String json = name_and_json.substring(space + 1);

    JsonDocument doc;
    const DeserializationError err = deserializeJson(doc, json);
    if (err.code() == DeserializationError::Ok) {
        effects.set_json(name, doc.as<JsonObject>());
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

    // audio.setup();
    midi.setup();

    universe = new Universe(*controller);

    //
    // Panning the mover light.
    //
    auto* pan = &effects.add_effect<CosBlend>("pan");
    pan->set_config(CosBlendConfig{.freq={0.1}});
    pan->set_values(200, 230); // sweep the room!
    universe->mover.y.set_value(10);
    universe->mover.x.add_effect(pan);
    pan->trigger(0);

    auto& shimmer = effects.add_effect<CompositeEffect<CosBlend>>("shimmer");
    Palette& palette = effects.add_effect<Palette>("palette");
    auto& solid = effects.add_effect<Solid>("rgb");

    uint32_t offset = 0;

    universe->litake[0].add_rgb_effect(palette.add_effect(offset));
    universe->litake[0].add_rgb_effect(solid);

    offset += 100;
    universe->litake[1].add_rgb_effect(palette.add_effect(offset));
    universe->litake[1].add_rgb_effect(solid);

    offset += 100;
    universe->missyee[0].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[0].add_rgb_effect(solid);

    offset += 300;
    universe->missyee[1].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[1].add_rgb_effect(solid);

    offset += 100;
    universe->mover.add_rgb_effect(palette.add_effect(offset));
    universe->mover.add_rgb_effect(solid);

    offset += 20;

    constexpr size_t NUM_DIVISIONS = 7;
    for (size_t i = 0; i < NUM_DIVISIONS; ++i) {
        size_t start = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * i;
        size_t end = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * (i + 1);
        offset += 50;
        auto& effect = palette.add_effect(offset);
        auto& light = universe->bar[0];
        for (size_t j = start; j < end; ++j) {
            light.white(j).add_effect(&shimmer);
            light.red(j).add_effect(&effect.red());
            light.green(j).add_effect(&effect.green());
            light.blue(j).add_effect(&effect.blue());
            light.red(j).add_effect(&solid.red());
            light.green(j).add_effect(&solid.green());
            light.blue(j).add_effect(&solid.blue());
        }
    }
    for (size_t i = 0; i < NUM_DIVISIONS; ++i) {
        size_t start = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * i;
        size_t end = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * (i + 1);
        offset += 50;
        auto& effect = palette.add_effect(offset);
        auto& light = universe->bar[1];
        for (size_t j = start; j < end; ++j) {
            light.white(j).add_effect(&shimmer);
            light.red(j).add_effect(&effect.red());
            light.green(j).add_effect(&effect.green());
            light.blue(j).add_effect(&effect.blue());
            light.red(j).add_effect(&solid.red());
            light.green(j).add_effect(&solid.green());
            light.blue(j).add_effect(&solid.blue());
        }
    }
    for (size_t i = 0; i < NUM_DIVISIONS; ++i) {
        size_t start = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * i;
        size_t end = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * (i + 1);
        offset += 50;
        auto& effect = palette.add_effect(offset);
        auto& light = universe->bar[2];
        for (size_t j = start; j < end; ++j) {
            light.white(j).add_effect(&shimmer);
            light.red(j).add_effect(&effect.red());
            light.green(j).add_effect(&effect.green());
            light.blue(j).add_effect(&effect.blue());
            light.red(j).add_effect(&solid.red());
            light.green(j).add_effect(&solid.green());
            light.blue(j).add_effect(&solid.blue());
        }
    }

    offset += 100;
    universe->missyee[2].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[2].add_rgb_effect(solid);

    offset += 100;
    universe->missyee[3].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[3].add_rgb_effect(solid);

    solid.trigger(millis());

    PaletteConfig config;
    config.type = PaletteConfig::TransitionType::STEP_SWEEP;
    const auto rgb = [](uint8_t r, uint8_t g, uint8_t b){
        return PaletteConfig::Color{.r=r / 255.0f, .g=g / 255.0f, .b=b / 255.0f};
    };
    config.palettes["red"] = {
        rgb(255, 0, 0),
        rgb(0, 0, 0),
    };
    config.palettes["hues"] = {
        rgb(255, 0, 0),
        rgb(0, 255, 0),
        rgb(0, 0, 255),
    };
    config.palettes["fire"] = {
        rgb(255, 0, 0),
        rgb(255, 25, 0),
        rgb(200, 50, 0),
        rgb(255, 100, 0),
    };
    config.palettes["neon"] = {
        rgb(255, 10, 10),
        rgb(0, 100, 255),
        rgb(76, 205, 153),
        rgb(255, 16, 240),
        rgb(0, 12, 200),
    };
    config.palettes["blues"] = {
        rgb(0, 0, 255),
        rgb(0, 100, 255),
        rgb(30, 25, 180),
        rgb(75, 0, 200),
        rgb(0, 75, 200),
    };

    config.palette = "red";

    config.fade_time_ms = 50;
    palette.set_config(config);

    //
    // Fade in on startup
    //
    auto& blank = effects.add_effect<Blank>("blank");
    for (auto& missyee : universe->missyee) {
        missyee.brightness.add_effect(&blank);
    }
    for (auto& litake : universe->litake) {
        litake.add_effect(blank);
    }
    universe->mover.add_effect(blank);
    for (auto& bar : universe->bar) {
        for (size_t l = 0; l < WashBarLight112::NUM_LIGHTS; ++l) {
            bar.red(l).add_effect(&blank);
            bar.green(l).add_effect(&blank);
            bar.blue(l).add_effect(&blank);
            bar.white(l).add_effect(&blank);
        }
    }
    blank.clear(0);
}

void loop() {
    periodic.tick();
    midi.read();
    // audio.read();
    interface.handle_serial();
    controller->write_frame();
}