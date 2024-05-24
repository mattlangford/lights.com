#pragma once

#include <vector>
#include <map>
#include <string>

struct PaletteConfig {
    struct Color {
        float r = 0.0;
        float g = 0.0;
        float b = 0.0;
    };

    // Describes what should happen on a trigger
    enum class TransitionType {
        RANDOM, // step each light to a random color
        STEP, // step all fixtures to the next color in the palette
        STEP_SWEEP, // sweep into the next color
    };

    TransitionType type = TransitionType::RANDOM;

    std::map<String, std::vector<Color>> palettes;
    String palette;

    uint32_t fade_time_ms = 1000;
};

class Palette : public EffectBase {
public:
    using Effect = RgbEffect<FaderEffect>;

public:
    Palette() { }
    Palette(PaletteConfig config) { set_config(std::move(config)); }
    ~Palette() override = default;

    String type() const { return "Palette"; }

    void set_config(PaletteConfig config) {
        config_ = std::move(config);

        // default to the first
        if (config_.palette == "" && !config_.palettes.empty()) {
            config_.palette = config_.palettes.begin()->first;
        }
    }

    SetConfigResult set_config_json(const JsonObject& json) override {
        SetConfigResult result = SetConfigResult::no_values_set();

        const auto& palette = json["palette"];
        if (!palette.isNull()) {
            String palette = json["palette"].as<String>();
            if (config_.palettes.find(palette) == config_.palettes.end()) {
                result.consider(SetConfigResult::error("Unknown palette type."));
            } else {
                result.consider(SetConfigResult::okay());
                config_.palette = palette;
            }
        }

        const auto& type = json["type"];
        if (!type.isNull()) {
            result.consider(SetConfigResult::okay());
            if (type == "RANDOM") {
                config_.type = PaletteConfig::TransitionType::RANDOM;
            } else if (type == "STEP") {
                config_.type = PaletteConfig::TransitionType::STEP;
            } else if (type == "STEP_SWEEP") {
                config_.type = PaletteConfig::TransitionType::STEP_SWEEP;
            } else {
                result.consider(SetConfigResult::error("Unknown palette type."));
            }
        }

        result.maybe_set(json, "fade_time_ms", config_.fade_time_ms);

        if (result.type != SetConfigResult::ErrorType::NO_VALUES) {
            trigger(now()); // Do a transition to the next color
        }
        return result;
    };

    void get_config_json(JsonObject& json) const override {
        json["palette"] = config_.palette;

        switch (config_.type) {
        case PaletteConfig::TransitionType::RANDOM:
            json["type"] = "RANDOM";
            break;
        case PaletteConfig::TransitionType::STEP:
            json["type"] = "STEP";
            break;
        case PaletteConfig::TransitionType::STEP_SWEEP:
            json["type"] = "STEP_SWEEP";
            break;
        }

        json["fade_time_ms"] = config_.fade_time_ms;
    };

    void trigger(uint32_t now_ms) {
        size_t to_index = 0;
        switch (config_.type) {
        case PaletteConfig::TransitionType::RANDOM:
            to_index = rand();
            break;
        case PaletteConfig::TransitionType::STEP:
        case PaletteConfig::TransitionType::STEP_SWEEP:
            to_index = index_++;
            break;
        }

        auto it = config_.palettes.find(config_.palette);
        if (it == config_.palettes.end()) {
            return;
        }
        const auto& palette = it->second;

        for (size_t l = 0; l < effects_.size(); ++l) {
            if (config_.type == PaletteConfig::TransitionType::RANDOM){
                to_index = rand();
            }

            const PaletteConfig::Color to_color = palette[to_index % palette.size()];
            uint32_t start_time = now_ms;
            if (config_.type == PaletteConfig::TransitionType::STEP_SWEEP) {
                start_time += offsets_[l];
            }
            const uint32_t end_time = start_time + config_.fade_time_ms;
            auto& effect = *effects_[l];

            effect.red().fade_to(to_color.r, start_time, end_time);
            effect.green().fade_to(to_color.g, start_time, end_time);
            effect.blue().fade_to(to_color.b, start_time, end_time);
        }
    }

    void clear(uint32_t now_ms) {
        for (auto& effect : effects_) {
            effect->clear(now_ms + config_.fade_time_ms);
        }
    }

    Effect& add_effect(uint32_t offset_ms=0) {
        offsets_.push_back(offset_ms);
        effects_.push_back(std::make_unique<Effect>());
        return *effects_.back();
    }

    String next_palette() {
        size_t index = 0;
        for (const auto& it : config_.palettes) {
            if (it.first == config_.palette) {
                break;
            }
            index++;
        }
        index = (index + 1) % config_.palettes.size();
        set_palette(std::next(config_.palettes.begin(), index)->first);
        return config_.palette;
    }
    void set_palette(String palette) {
        config_.palette = palette;

        // Fast fade to this color
        auto fade = config_.fade_time_ms;
        config_.fade_time_ms = 100;

        trigger(millis());

        config_.fade_time_ms = fade;
    }

private:
    PaletteConfig config_;
    size_t index_ = 0;

    std::vector<std::unique_ptr<Effect>> effects_;
    std::vector<uint32_t> offsets_;
};

class Solid final : public RgbEffect<FaderEffect> {
public:
    SetConfigResult set_config_json(const JsonObject& json) override {
        SetConfigResult result = SetConfigResult::no_values_set();

        float r, g, b;
        if (result.maybe_set(json, "r", r)) {
            red().set_min(r);
            red().set_max(r);
        }
        if (result.maybe_set(json, "g", g)) {
            green().set_min(g);
            green().set_max(g);
        }
        if (result.maybe_set(json, "b", b)) {
            blue().set_min(b);
            blue().set_max(b);
        }
        return result;
    };

    void get_config_json(JsonObject& json) const override {
        json["r"] = red().min_value();
        json["g"] = green().min_value();
        json["b"] = blue().min_value();
    };
};