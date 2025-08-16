#include "dmx.hh"
#include "effects.hh"
#include "lights.hh"
#include "cli.hh"

#include <map>
#include <set>
#include <vector>
#include <memory>
#include <vector>
#include <random>

DMXController* controller;
Interface interface;
EffectMap effects;
PeriodicTrigger<Palette>* palette_trigger;
uint8_t cc_1_last_value = 0;
uint8_t cc_2_last_value = 0;
std::array<LinearPulse*, 9> sparkles;

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
uint32_t tick_timer[COUNT] {0};
uint32_t update_timer[COUNT] {0};
uint32_t periodic_timer[COUNT] {0};
uint32_t midi_manager_timer[COUNT] {0};
uint32_t interface_timer[COUNT] {0};
uint32_t controller_timer[COUNT] {0};

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

    for (uint32_t mic : update_timer) avg += static_cast<float>(mic) / static_cast<float>(COUNT);
    Serial.print(" Update Timer (us): ");
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
    avg = 0.0;

    for (uint32_t mic : tick_timer) avg += static_cast<float>(mic) / static_cast<float>(COUNT);
    Serial.print(" Tick Timer (us): ");
    Serial.println(avg);
}
void show_midi(const String&) {
    Serial.print(" CC1: ");
    Serial.println(cc_1_last_value);
    Serial.print(" CC2: ");
    Serial.println(cc_2_last_value);
    Serial.print(" BPM: ");
    Serial.println(midi_manager.bpm());
}

void read_midi() {
    midi_manager.read(1);
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
    interface.add_handler("midi", "Shows midi status", &show_midi);

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

    auto add_bar_light = [&](size_t index, std::map<uint8_t, LinearPulse*> kicks) {
        universe->bar[index].add_rgb_effect(palette.add_effect(offset));
        constexpr size_t NUM_DIVISIONS = 7;
        for (size_t i = 0; i < NUM_DIVISIONS; ++i) {
            size_t start = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * i;
            size_t end = WashBarLight112::NUM_LIGHTS / NUM_DIVISIONS * (i + 1);

            offset += 20;
            auto& kick = kick_palette.add_effect(offset);
            auto& light = universe->bar[index];
            for (size_t j = start; j < end; ++j) {
                light.add_rgb_effect(j, kick);
                light.add_effect(j, twinkle.add());
            }
        }
        universe->bar[index].add_rgb_effect(solid);
    };

    add_bar_light(0, {{2, &kick_4}, {6, &kick_6}});
    add_bar_light(1, {{4, &kick_1}, {2, &kick_3}});
    add_bar_light(2, {{1, &kick_5}, {3, &kick_2}});

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

    size_t index = 0;
    for (auto& bar : universe->bar) {
        for (size_t i = 0; i < 3; ++i, ++index) {
            sparkles[index] = &effects.add_effect<LinearPulse>("sparkle" + String(index));
            sparkles[index]->set_config(LinearPulseConfig{
                .rise_dt_ms=1000,
                .fall_dt_ms=2000
            });

            constexpr size_t NUM_LIGHTS = WashBarLight112::NUM_LIGHTS;
            for (size_t l = i * NUM_LIGHTS / 3; l < NUM_LIGHTS * (i + 1) / 3; ++l) {
                bar.green(l).add_effect(sparkles[index]);
            }
        }
    }

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

    auto& cc_2 = effects.add_effect<SingleChannelEffect>("cc_2");
    cc_2.set_input_gain(0.0);
    for (auto& bar : universe->bar) {
        for (size_t l = WashBarLight112::NUM_LIGHTS / 3; l < 2 * WashBarLight112::NUM_LIGHTS / 3; l++) {
            bar.red(l).add_effect(&cc_2);
            bar.green(l).add_effect(&cc_2);
            bar.blue(l).add_effect(&cc_2);
        }
    }

    blank.clear(millis() + 1000);
    palette.trigger(millis());
    solid.trigger(millis());
    twinkle.trigger(millis());

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
        [&kick_palette_trigger, w=false, n = MidiManager::MidiNote{'D', 3}.note()]
        (byte channel, byte note, byte velocity, bool on) mutable {
        if (on && note == n) {
            Serial.print("Calm mode ");
            Serial.println(w ? "ON" : "OFF");

            palette_trigger->set_enabled(w);
            kick_palette_trigger.set_enabled(!w);
            w = !w;
        }
    });
    midi_manager.add_callback([&cc_2](byte channel, byte note) mutable {
        float value = 0.1 + 0.9 * (static_cast<float>(note) / 128.f);
        if (channel == 1) {
            cc_1_last_value = note;
            // cc_1.set_input_gain(value);
        } else if (channel == 2) {
            cc_2_last_value = note;
            cc_2.set_input_gain(value);
        }
    });
}

void loop() {
    uint32_t tick_start = micros();
    uint32_t start, end;

    start = micros();
    static uint32_t palette_last_update = millis();
    uint32_t now = millis();
    if (now > (palette_last_update + 1000)) {
        palette_last_update = now;
        // bpm is quarter notes per minute, so set the rate to millseconds per beat
        uint32_t rate_ms = 8 * (60000.0f / max(1E-3f, midi_manager.bpm()));
        palette_trigger->set_rate(rate_ms);
    }

    static uint32_t sparkle_last_update = millis();
    if (now > (sparkle_last_update + 100)) {
        sparkle_last_update = now;
        static std::mt19937_64 rng(42);
        static const double LAMBDA = std::log(3.0/2.0);
        static std::uniform_real_distribution<double> dist;
        // Serial.println(cc_1_last_value);
        const double p = 1.0 - std::exp(-LAMBDA * (static_cast<double>(cc_1_last_value) / 128.0) * 0.1);
        if (dist(rng) < p) {
            static std::uniform_int_distribution<size_t> dist(0, sparkles.size() - 1);
            sparkles[dist(rng)]->trigger(now);
        }
    }
    end = micros();
    update_timer[i] = end - start;

    start = end;
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

    tick_timer[i] = micros() - tick_start;

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