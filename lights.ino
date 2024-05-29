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
    pan->set_config(CosBlendConfig{.freq={0.25}});
    pan->set_values(40, 80); // sweep the room!
    universe->mover.y.set_value(10);
    universe->mover.x.add_effect(pan);
    pan->trigger(0);

    auto& kick_palette_trigger = effects.add_effect<MidiTrigger<Palette>>("kick_palette", 'C', 2);
    auto& kick_palette = kick_palette_trigger.effect();

    auto& palette_trigger = effects.add_effect<PeriodicTrigger<Palette>>("palette", 4000);
    palette_trigger.set_enabled(false);
    auto& palette = palette_trigger.effect();
    auto& solid = effects.add_effect<Solid>("rgb");
    auto& twinkle = effects.add_effect<CompositeEffect<Twinkle>>("twinkle");

    uint32_t offset = 0;

    universe->litake[0].add_rgb_effect(kick_palette.add_effect(offset));
    universe->litake[0].add_rgb_effect(palette.add_effect(offset));
    universe->litake[0].add_rgb_effect(solid);
    universe->litake[0].add_effect(twinkle.add());

    offset += 100;
    universe->litake[1].add_rgb_effect(kick_palette.add_effect(offset));
    universe->litake[1].add_rgb_effect(palette.add_effect(offset));
    universe->litake[1].add_rgb_effect(solid);
    universe->litake[1].add_effect(twinkle.add());

    offset += 100;

    universe->missyee[0].add_rgb_effect(kick_palette.add_effect(offset));
    universe->missyee[0].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[0].add_rgb_effect(solid);
    universe->missyee[0].add_effect(twinkle.add());

    offset += 300;
    universe->missyee[1].add_rgb_effect(kick_palette.add_effect(offset));
    universe->missyee[1].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[1].add_rgb_effect(solid);
    universe->missyee[1].add_effect(twinkle.add());

    offset += 100;
    auto& kick_pulse_trigger = effects.add_effect<MidiTrigger<LinearPulse>>("kick_pulse", 'C', 2);
    kick_pulse_trigger.set_every_n(2);
    auto& kick_pulse = kick_pulse_trigger.effect();
    kick_pulse.set_config(LinearPulseConfig{
        .rise_dt_ms=10,
        .hold_dt_ms=100,
        .fall_dt_ms=500
    });
    kick_pulse.set_multiply(true);
    kick_pulse.set_min(100);
    universe->mover.add_rgb_effect(kick_palette.add_effect(offset));
    universe->mover.add_rgb_effect(palette.add_effect(offset));
    universe->mover.add_effect(kick_pulse);

    offset += 20;

    auto& kick_step = effects.add_effect<MidiTrigger<StepTrigger<LinearPulse>>>("kick_step", 'C', 2).effect();
    auto& kick_1 = kick_step.add();
    auto& kick_2 = kick_step.add();
    auto& kick_3 = kick_step.add();
    auto& kick_4 = kick_step.add();
    auto& kick_5 = kick_step.add();
    auto& kick_6 = kick_step.add();
    kick_step.set_config(LinearPulseConfig{
        .rise_dt_ms=10,
        .hold_dt_ms=100,
        .fall_dt_ms=1000
    });

    auto add_bar_light = [&](size_t index, std::map<uint8_t, LinearPulse*> lights){
        constexpr size_t NUM_DIVISIONS = 7;
        for (size_t i = 0; i < NUM_DIVISIONS; ++i) {
            size_t start = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * i;
            size_t end = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * (i + 1);

            offset += 20;
            auto& effect = kick_palette.add_effect(offset);
            auto& light = universe->bar[index];
            auto it = lights.find(i);
            for (size_t j = start; j < end; ++j) {
                if (it != lights.end()) {
                    light.white(j).add_effect(it->second);
                }
                light.add_rgb_effect(j, effect);
                light.add_effect(j, twinkle.add());
            }
        }
        universe->bar[index].add_rgb_effect(palette.add_effect(offset));
        universe->bar[index].add_rgb_effect(solid);
    };

    add_bar_light(0, {{2, &kick_4}, {6, &kick_6}});
    add_bar_light(1, {{4, &kick_1}, {2, &kick_3}});
    add_bar_light(2, {{1, &kick_5}, {3, &kick_2}});

    offset += 100;

    auto& hat_pulse = effects.add_effect<MidiTrigger<LinearPulse>>("hat_pulse", 'D', 2).effect();
    hat_pulse.set_config(LinearPulseConfig{
        .rise_dt_ms=10,
        .hold_dt_ms=100,
        .fall_dt_ms=100
    });
    hat_pulse.set_min(100);
    universe->missyee[2].add_rgb_effect(kick_palette.add_effect(offset));
    universe->missyee[2].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[2].add_rgb_effect(solid);
    universe->missyee[2].add_effect(twinkle.add());
    universe->missyee[2].brightness.set_value(0);
    universe->missyee[2].brightness.add_effect(&hat_pulse);

    offset += 100;
    universe->missyee[3].add_rgb_effect(kick_palette.add_effect(offset));
    universe->missyee[3].add_rgb_effect(palette.add_effect(offset));
    universe->missyee[3].add_rgb_effect(solid);
    universe->missyee[3].add_effect(twinkle.add());

    PaletteConfig config;
    config.type = PaletteConfig::TransitionType::RANDOM;
    const auto rgb = [](uint8_t r, uint8_t g, uint8_t b){
        return PaletteConfig::Color{.r=r / 255.0f, .g=g / 255.0f, .b=b / 255.0f};
    };
    config.palettes["red"] = {
        rgb(255, 0, 0),
        rgb(0, 0, 0),
    };
    config.palettes["yed"] = {
        rgb(255, 0, 0),
        rgb(255, 100, 0),
    };
    config.palettes["bled"] = {
        rgb(0, 0, 255),
        rgb(255, 0, 0),
    };
    config.palettes["hues"] = {
        rgb(255, 0, 0),
        rgb(128, 128, 0),
        rgb(0, 255, 0),
        rgb(0, 128, 128),
        rgb(0, 0, 255),
    };
    config.palettes["fire"] = {
        rgb(255, 0, 0),
        rgb(200, 0, 0),
        rgb(255, 25, 0),
        rgb(230, 10, 0),
        rgb(200, 50, 0),
        rgb(255, 60, 0),
        rgb(230, 75, 0),
        rgb(255, 50, 0),
        rgb(255, 40, 0),
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
    config.palettes["green"] = {
        rgb(0, 255, 0),
        rgb(0, 255, 25),
        rgb(75, 255, 0),
        rgb(128, 128, 0),
        rgb(50, 200, 0),
        rgb(200, 128, 0),
        rgb(255, 230, 0),
    };

    config.palette = "neon";

    config.fade_time_ms = 250;
    kick_palette.set_config(config);

    config.fade_time_ms = 2000;
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

    blank.clear(millis() + 1000);
    palette.trigger(millis());
    solid.trigger(millis());
    twinkle.trigger(millis());

    midi.add_callback([&palette,
                       &kick_palette,
                       w=true,
                       n = MidiManager::MidiNote{'C', 3}.note()](byte channel, byte note, byte velocity, bool on){
        if (on && note == n) {
            auto p = palette.next_palette();
            kick_palette.set_palette(p);

            Serial.print("Switching to ");
            Serial.println(p);
        }
    });
    midi.add_callback([&palette_trigger,
                       &kick_palette_trigger,
                       w=true,
                       n = MidiManager::MidiNote{'D', 3}.note()](byte channel, byte note, byte velocity, bool on) mutable {
        if (on && note == n) {
            palette_trigger.set_enabled(w);
            kick_palette_trigger.set_enabled(!w);
            w = !w;
        }
    });

}

void loop() {
    periodic.tick();
    midi.read();
    // audio.read();
    interface.handle_serial();
    controller->write_frame();
}