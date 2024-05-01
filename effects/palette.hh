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
        RANDOM, // step all fixtures to a random color in the palette
        STEP, // step all fixtures to the next color in the palette
        UNIQUE_RANDOM, // step each fixture to a random color in the palette
        STEP_SWEEP, // sweep into the next color
    };

    TransitionType type = TransitionType::RANDOM;

    std::map<std::string, std::vector<Color>> palettes;
    std::string palette;

    uint32_t fade_time_ms = 100;
};

class Palette : public EffectBase {
public:
    using Effect = RgbEffect<FaderEffect>;

    // A single fixture within the palette, each fixture may contain multiple lights (like a bar)
    class Fixture {
    public:
        Fixture(uint32_t offset_ms) : offset_(offset_ms) {}

        Effect& add_effect(size_t offset_ms) {
            offsets_.push_back(offset_ms);
            effects_.emplace_back(std::make_unique<Effect>());
            return *effects_.back();
        };

        Effect& effect(size_t index) { return *effects_[index]; }
        uint32_t offset(size_t index) const { return offset_ + offsets_[index]; };
        size_t size() const { return effects_.size(); }

    private:
        std::vector<std::unique_ptr<Effect>> effects_;
        std::vector<uint32_t> offsets_;
        uint32_t offset_;
    };

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
            std::string palette = json["palette"].as<std::string>();
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
            } else if (type == "UNIQUE_RANDOM") {
                config_.type = PaletteConfig::TransitionType::UNIQUE_RANDOM;
            } else {
                result.consider(SetConfigResult::error("Unknown palette type."));
            }
        }

        result.maybe_set(json, "fade_time_ms", config_.fade_time_ms);
        trigger(now()); // Do a transition to the next color
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
        case PaletteConfig::TransitionType::UNIQUE_RANDOM:
            json["type"] = "UNIQUE_RANDOM";
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
        case PaletteConfig::TransitionType::UNIQUE_RANDOM:
            break;
        }

        auto it = config_.palettes.find(config_.palette);
        if (it == config_.palettes.end()) {
            return;
        }
        const auto& palette = it->second;

        for (auto& fixture : fixtures_) {
            for (size_t l = 0; l < fixture.size(); ++l) {
                if (config_.type == PaletteConfig::TransitionType::UNIQUE_RANDOM){
                    to_index = rand();
                }

                const PaletteConfig::Color to_color = palette[to_index % palette.size()];
                uint32_t start_time = now_ms;
                if (config_.type == PaletteConfig::TransitionType::STEP_SWEEP) {
                    start_time += fixture.offset(l);
                }
                const uint32_t end_time = start_time + config_.fade_time_ms;
                auto& effect = fixture.effect(l);

                effect.red().fade_to(to_color.r, start_time, end_time);
                effect.green().fade_to(to_color.g, start_time, end_time);
                effect.blue().fade_to(to_color.b, start_time, end_time);
            }
        }
    }

    void clear(uint32_t now_ms) {
        for (auto& fixture : fixtures_) {
            for (size_t l = 0; l < fixture.size(); ++l) {
                auto& effect = fixture.effect(l);
                effect.red().fade_to(0.0, now_ms, now_ms + config_.fade_time_ms);
                effect.green().fade_to(0.0, now_ms, now_ms + config_.fade_time_ms);
                effect.blue().fade_to(0.0, now_ms, now_ms + config_.fade_time_ms);
            }
        }
    }

    Fixture& add_fixture(uint32_t offset_ms=0) {
        fixtures_.push_back(offset_ms);
        return fixtures_.back();
    }

private:
    PaletteConfig config_;
    size_t index_ = 0;

    std::vector<Fixture> fixtures_;
};
