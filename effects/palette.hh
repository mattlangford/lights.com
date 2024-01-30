#pragma once

#include <vector>

struct PaletteConfig {
    struct Color {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
    };

    // Describes what should happen on a trigger
    enum class TransitionType {
        RANDOM, // step all fixtures to a random color in the palette
        STEP, // step all fixtures to the next color in the palette
        UNIQUE_RANDOM, // step each fixtures to a random color in the palette
    };

    TransitionType type = TransitionType::RANDOM;

    std::vector<Color> palette;

    uint32_t fade_time_ms = 0;
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
    ~Palette() override = default;

    String type() const { return "Palette"; }

    void set_config_json(const JsonObject& json) override {};
    void get_config_json(JsonObject& json) const override {};

    void trigger(uint32_t now_ms) {
        // We might use this if we need to.
        const size_t random_index = rand() % config_.palette.size();

        for (auto& fixture : fixtures_) {
            for (size_t l = 0; l < fixture.size(); ++l) {
                PaletteConfig::Color to_color;
            }
        }
    }
    void clear(uint32_t now_ms) {}

    Fixture& add_fixture(uint32_t offset_ms=0) {
        fixtures_.push_back(offset_ms);
        return fixtures_.back();
    }

private:
    PaletteConfig config_;
    size_t index_ = 0;

    std::vector<Fixture> fixtures_;
};