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
PeriodicTrigger<Palette>* palette_trigger;

struct Universe {
    LitakeLight litake[2];
    MissyeeLight missyee[4];
    WashBarLight112 bar[3];

    Universe(DMXController& controller):
        litake{{5, controller}, {1, controller}},
        missyee{{10, controller}, {17, controller}, {31, controller}, {24, controller}},
        bar{{162, controller}, {274, controller}, {50, controller}} {

        controller.set_max_channel(3 * 112);
    }
};
Universe* universe;

constexpr size_t COUNT = 100;
static size_t i = 0;
uint32_t periodic_timer[COUNT];
uint32_t midi_manager_timer[COUNT];
uint32_t interface_timer[COUNT];
uint32_t controller_timer[COUNT];

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
void timers(const String&) {
    float avg = 0.0;
    for (uint32_t mic : periodic_timer) avg += static_cast<float>(mic) / static_cast<float>(COUNT);
    Serial.print(" Periodic Timer (us): ");
    Serial.println(avg);
    avg = 0.0;

    for (uint32_t mic : midi_manager_timer) avg += static_cast<float>(mic) / static_cast<float>(COUNT);
    Serial.print(" Midi Timer (us): ");
    Serial.println(avg);
    avg = 0.0;

    for (uint32_t mic : interface_timer) avg += static_cast<float>(mic) / static_cast<float>(COUNT);
    Serial.print(" Interface Timer (us): ");
    Serial.println(avg);
    avg = 0.0;

    for (uint32_t mic : controller_timer) avg += static_cast<float>(mic) / static_cast<float>(COUNT);
    Serial.print(" Controller Timer (us): ");
    Serial.println(avg);
}
void bpm(const String&) {
    Serial.println(midi_manager.bpm());
}

void read_midi() {
    midi_manager.read();
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    interface.add_handler("list", "show current configuration", &list);
    interface.add_handler("trigger", "trigger the specified effect", &trigger);
    interface.add_handler("clear", "clear the specified effect", &clear);
    interface.add_handler("set", "sets current config from a JSON string", &set);
    interface.add_handler("values", "gets the current values of all channels", &values);
    interface.add_handler("timers", "show the latest timer data", &timers);
    interface.add_handler("bpm", "The current estimated BPM", &bpm);

    controller = new DMXController(41, 40);

    midi_manager.setup();

    universe = new Universe(*controller);

    auto& kick_palette_trigger = effects.add_effect<MidiTrigger<Palette>>("kick_palette", 'C', 2);
    auto& kick_palette = kick_palette_trigger.effect();

    // Save this to clock it to the BPM
    palette_trigger = &effects.add_effect<PeriodicTrigger<Palette>>("palette", 4000);
    palette_trigger->set_enabled(true);
    auto& palette = palette_trigger->effect();
    auto& solid = effects.add_effect<Solid>("rgb");
    auto& twinkle = effects.add_effect<CompositeEffect<Twinkle>>("twinkle");

    uint32_t offset = 0;

    offset += 100;
    auto& snare_pulse_trigger = effects.add_effect<MidiTrigger<LinearPulse>>("snare_pulse", 'C', 2, true);
    snare_pulse_trigger.set_every_n(2);
    snare_pulse_trigger.set_enabled(false);
    auto& snare_pulse = snare_pulse_trigger.effect();
    snare_pulse.set_config(LinearPulseConfig{
        .rise_dt_ms=10,
        .hold_dt_ms=100,
        .fall_dt_ms=500
    });
    snare_pulse.set_multiply(true);
    snare_pulse.set_min(100);

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

    auto& hat_trigger = effects.add_effect<MidiTrigger<LinearPulse>>("hat_pulse", 'D', 2);
    auto& hat_pulse = hat_trigger.effect();
    hat_pulse.set_config(LinearPulseConfig{
        .rise_dt_ms=10,
        .hold_dt_ms=100,
        .fall_dt_ms=100
    });
    hat_trigger.set_enabled(false);
    hat_pulse.set_min(10);

    auto add_bar_light = [&](size_t index, std::map<uint8_t, LinearPulse*> kicks, std::set<uint8_t> hat){
        universe->bar[index].add_rgb_effect(palette.add_effect(offset));
        constexpr size_t NUM_DIVISIONS = 7;
        for (size_t i = 0; i < NUM_DIVISIONS; ++i) {
            size_t start = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * i;
            size_t end = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * (i + 1);

            offset += 20;
            auto& kick = kick_palette.add_effect(offset);
            auto& light = universe->bar[index];
            // auto it = kicks.find(i);
            for (size_t j = start; j < end; ++j) {
                // TODO: Disable this effect
                // if (it != kicks.end()) {
                //     light.white(j).add_effect(it->second);
                // }
                if (hat.find(i) != hat.end()) {
                    light.red(j).add_effect(&hat_pulse);
                    light.green(j).add_effect(&hat_pulse);
                }
                light.add_rgb_effect(j, kick);
                light.add_effect(j, twinkle.add());
            }
        }
        universe->bar[index].add_rgb_effect(solid);
    };

    add_bar_light(0, {{2, &kick_4}, {6, &kick_6}}, {1});
    add_bar_light(1, {{4, &kick_1}, {2, &kick_3}}, {});
    add_bar_light(2, {{1, &kick_5}, {3, &kick_2}}, {});

    offset += 100;

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

    config.fade_time_ms = 1000;
    palette.set_config(config);

    //
    // Fade in on startup
    //
    auto& blank = effects.add_effect<Blank>("blank");
    for (auto& bar : universe->bar) {
        for (size_t l = 0; l < WashBarLight112::NUM_LIGHTS; ++l) {
            bar.red(l).add_effect(&blank);
            bar.green(l).add_effect(&blank);
            bar.blue(l).add_effect(&blank);
            bar.white(l).add_effect(&blank);
        }
    }

    auto& cc_1 = effects.add_effect<Blank>("cc_1");
    auto& cc_2 = effects.add_effect<Blank>("cc_2");
    for (auto& bar : universe->bar) {
        size_t l = 0;
        for (; l < WashBarLight112::NUM_LIGHTS / 3; l++) {
            bar.red(l).add_effect(&cc_1);
            bar.green(l).add_effect(&cc_1);
            bar.blue(l).add_effect(&cc_1);
            bar.white(l).add_effect(&cc_1);
        }
        for (; l < 2 * WashBarLight112::NUM_LIGHTS / 3; l++) {
            bar.red(l).add_effect(&cc_2);
            bar.green(l).add_effect(&cc_2);
            bar.blue(l).add_effect(&cc_2);
            bar.white(l).add_effect(&cc_2);
        }
        for (; l < WashBarLight112::NUM_LIGHTS; l++) {
            bar.red(l).add_effect(&cc_1);
            bar.green(l).add_effect(&cc_1);
            bar.blue(l).add_effect(&cc_1);
            bar.white(l).add_effect(&cc_1);
        }
    }


    blank.clear(millis() + 1000);
    palette.trigger(millis());
    solid.trigger(millis());
    twinkle.trigger(millis());
    cc_1.trigger(millis());
    cc_2.trigger(millis());

    // TODO: This sets the light up in an interesting pattern
    kick_palette_trigger.trigger(millis());
    kick_palette_trigger.set_enabled(false);

    midi_manager.add_callback(
        [&palette, &kick_palette, w=true, n = MidiManager::MidiNote{'C', 3}.note()]
        (byte channel, byte note, byte velocity, bool on){
        if (on && note == n) {
            auto p = palette.next_palette();
            kick_palette.set_palette(p);

            Serial.print("Switching to ");
            Serial.println(p);
        }
    });
    midi_manager.add_callback(
        [&kick_palette_trigger, &hat_trigger, w=false, n = MidiManager::MidiNote{'D', 3}.note()]
        (byte channel, byte note, byte velocity, bool on) mutable {
        if (on && note == n) {
            Serial.print("Calm mode ");
            Serial.println(w ? "ON" : "OFF");

            palette_trigger->set_enabled(w);
            hat_trigger.set_enabled(!w);
            kick_palette_trigger.set_enabled(!w);
            w = !w;
        }
    });

    midi_manager.add_callback([&cc_1, &cc_2](byte channel, byte note) mutable {
            float value = 0.1 + 0.9 * (static_cast<float>(note) / 128.f);
            if (channel == 1) {
                cc_1.set_input_gain(value);
            } else if (channel == 2) {
                cc_2.set_input_gain(value);
            }
    });
}

void loop() {
    uint32_t start, end;

    static uint32_t last_update = millis();
    uint32_t now = millis();
    if (now > (last_update + 1000)) {
        last_update = now;
        // bpm is quarter notes per minute, so set the rate to millseconds per beat
        uint32_t rate_ms = 3 * (60000.0f / max(1E-3f, midi_manager.bpm()));
        palette_trigger->set_rate(rate_ms);
    }

    start = micros();
    periodic.tick();
    end = micros();
    periodic_timer[i] = end - start;

    start = end;
    midi_manager.read();
    end = micros();
    midi_manager_timer[i] = end - start;

    start = end;
    interface.handle_serial();
    end = micros();
    interface_timer[i] = end - start;

    start = end;
    controller->write_frame(&read_midi);
    end = micros();
    controller_timer[i] = end - start;

    // Reset the timers
    if (i++ > COUNT) {
        i = 0;
    }
}

/*
Speed of twinkle associated with clock 24 pulse per quater note

Presets:
 * Neon

Two modes:
 * Calm (no drums!)
 * Kick drum 

Control signal

*/