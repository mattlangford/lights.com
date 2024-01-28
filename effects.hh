#pragma once

#include "dmx.hh"

#include <ArduinoJson.h>

#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <algorithm>
#include <type_traits>

class EffectBase {
public:
    virtual ~EffectBase() = default;

    virtual String type() const = 0;

    virtual void set_config_json(const JsonObject& json) = 0;
    virtual void get_config_json(JsonObject& json) const = 0;

    virtual void trigger(uint32_t now_ms) {}
    virtual void clear(uint32_t now_ms) {}

protected:
    uint32_t now() const { return millis(); }

    template <typename T>
    bool maybe_set(const JsonObject& obj, const char* name, T& out) {
        auto field = obj[name];
        if (field.isNull()) {
            return false;
        }
        out = field.as<T>();
        return true;
    }

    template <typename T>
    T value_or(const JsonObject& obj, const char* name, const T& fallback) {
        auto field = obj[name];
        if (field.isNull()) {
            return fallback;
        }
        return field.as<T>();
    }
};

class SingleChannelEffect : public ChannelEffect, public EffectBase {
public:
    ~SingleChannelEffect() override = default;

    // Provide min/max value overloads for config
    void set_config_json(const JsonObject& json) override {
        set_min(value_or(json, "min", min_value()));
        set_max(value_or(json, "max", max_value()));
        set_input_gain(value_or(json, "gain", input_gain()));
    }
    void get_config_json(JsonObject& json) const override {
        json["min"] = min_value();
        json["max"] = max_value();
        json["gain"] = input_gain();
    }
};

class FaderEffect : public SingleChannelEffect {
private:
    struct FadePoint {
        uint32_t time_ms = 0;
        float value = 0.0;
    };

public:
    ~FaderEffect() override = default;

    void fade_to(float end, uint32_t start_ms, uint32_t end_ms) {
        // If we're starting from scratch or interleveling, reset the future.
        if (fades_.empty() || start_ms < fades_.back().time_ms) {
            // Grab current level before clearing
            float start_level = level_impl(start_ms);
            fades_.clear();

            // Fade from the current value to the specified value
            fades_.push_back(FadePoint{start_ms, start_level});
            fades_.push_back(FadePoint{end_ms, end});
            return;
        }

        // Make sure the start time has a bookmark
        if (start_ms != fades_.back().time_ms) {
            fades_.push_back(FadePoint{start_ms, level_impl(start_ms)});
        }

        fades_.push_back(FadePoint{end_ms, end});
    }

    void clear(uint32_t now_ms) override {
        FadePoint now{now_ms, level(now_ms)};
        fades_.clear();
        fades_.emplace_back(std::move(now));
    }

    bool is_done(uint32_t now_ms) const {
        return fades_.empty() || now_ms > fades_.back().time_ms;
    }

protected:
    float level(uint32_t now_ms) {
        const float value = level_impl(now_ms);

        // Pruning logic
        if (fades_.size() > 1 && now_ms > 1000 + fades_.front().time_ms) {
            size_t i = 0;
            for (; i < fades_.size() && fades_[i].time_ms < now_ms; ++i) { }
            // Start at j=1 so we keep the most recent timestamp before now.
            for (size_t j = 1; j < i; ++j) {
                fades_.pop_front();
            }
        }

        return value;
    }

private:
    float level_impl(uint32_t now_ms) const {
        if (fades_.empty()) return 0.0;
        if (now_ms < fades_.front().time_ms) return fades_.front().value;
        if (now_ms >= fades_.back().time_ms) return fades_.back().value;

        for (size_t i = 0; i < fades_.size() - 1; ++i) {
            const FadePoint& next = fades_[i + 1];
            if (now_ms > next.time_ms) {
                continue;
            }

            const FadePoint& prev = fades_[i];
            const float percent = (static_cast<float>(now_ms) - prev.time_ms) / (next.time_ms - prev.time_ms);
            const float result = prev.value + percent * (next.value - prev.value);
            return result;
        }

        return fades_.back().value;
    }
    std::deque<FadePoint> fades_;
};


///
/// @brief Composite Effect where each sug effect is statically typed, and configured in the same way.
///
template <typename Effect, size_t Count = 0>
class CompositeEffect : public EffectBase {
public:
    ~CompositeEffect() override = default;

    String type() const override { return "CompositeEffect(" + subtype() + ")"; }
    String subtype() const { return effects_.empty() ? "?" : effect(0)->type(); }

    template <size_t C = Count, std::enable_if_t<C == 0, bool> = true>
    Effect& add() {
        effects_.push_back(std::make_unique<Effect>());
        return *effects_.back();
    }

    void set_config_json(const JsonObject& json) override {
        for (size_t i = 0; i < effects_.size(); ++i) {
            effect(i)->set_config_json(json);
        }
    }

    void get_config_json(JsonObject& json) const override {
        // Expecting these all to the same, just pick the first
        if (const Effect* e = effect(0)) {
            e->get_config_json(json);
        }
    };

    void trigger(uint32_t now_ms) override {
        for (size_t i = 0; i < effects_.size(); ++i) {
            effect(i)->trigger(now_ms);
        }
    }
    void clear(uint32_t now_ms) override {
        for (size_t i = 0; i < effects_.size(); ++i) {
            effect(i)->clear(now_ms);
        }
    }

    template <size_t C = Count, std::enable_if_t<C == 0, bool> = true>
    Effect* effect(size_t index) { if (index >= size()) return nullptr; return effects_[index].get(); }
    template <size_t C = Count, std::enable_if_t<C != 0, bool> = true>
    Effect* effect(size_t index) { if (index >= size()) return nullptr; return &effects_[index]; }
    template <size_t C = Count, std::enable_if_t<C == 0, bool> = true>
    const Effect* effect(size_t index) const { if (index >= size()) return nullptr; return effects_[index].get(); }
    template <size_t C = Count, std::enable_if_t<C != 0, bool> = true>
    const Effect* effect(size_t index) const { if (index >= size()) return nullptr; return &effects_[index]; }

    template <typename Config>
    void set_config(const Config& config) {
        for (size_t i = 0; i < effects_.size(); ++i) {
            effect(i)->set_config(config);
        }
    }

    size_t size() const { return effects_.size(); }

private:
    // Statically sized if the Count is known, otherwise use a vector
    using Container = std::conditional_t<Count == 0,
            std::vector<std::unique_ptr<Effect>>,
            std::array<Effect, Count>>;
    Container effects_;
};

///
/// @brief Composite Effect where each channel is split out
///
template <typename Effect>
class RgbEffect final : public CompositeEffect<Effect, 3> {
public:
    RgbEffect() = default;
    ~RgbEffect() override = default;

public:
    String type() const override { return "RGB(" + this->subtype() + ")"; }

    Effect& red() { return *this->effect(0); }
    Effect& green() { return *this->effect(1); }
    Effect& blue() { return *this->effect(2); }
    const Effect& red() const { return *this->effect(0); }
    const Effect& green() const { return *this->effect(1); }
    const Effect& blue() const { return *this->effect(2); }

    // Split configs out by name
    void set_config_json(const JsonObject& json) override {
        red().set_config_json(json["red"]);
        green().set_config_json(json["green"]);
        blue().set_config_json(json["blue"]);
    }

    void get_config_json(JsonObject& json) const override {
        JsonObject r = json.createNestedObject("red");
        red().get_config_json(r);
        JsonObject g = json.createNestedObject("green");
        green().get_config_json(g);
        JsonObject b = json.createNestedObject("blue");
        blue().get_config_json(b);
    };
};

// Copied from ChatGPT
void hsv_to_rgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    // Convert HSV values to the range [0, 1]
    float h_normalized = h / 255.0;
    float s_normalized = s / 255.0;

    int i = static_cast<int>(h_normalized * 6);
    float f = (h_normalized * 6) - i;

    // Each of these will be between 0 and 255
    float p = static_cast<uint8_t>(v * (1 - s_normalized));
    float q = static_cast<uint8_t>(v * (1 - f * s_normalized));
    float t = static_cast<uint8_t>(v * (1 - (1 - f) * s_normalized));

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = 0; g = 0; b = 0;
    }
}

///
/// @brief An effect that allows for a parent/child configuration relationship.
///
template <typename Effect>
class NestedEffect : public EffectBase {
public:
    ~NestedEffect() override = default;

public:
    Effect& effect() { return effect_; }
    const Effect& effect() const { return effect_; }

    String type() const override { return parent_type() + "(" + effect_.type() + ")"; }

    void set_config_json(const JsonObject& json) override {
        set_parent_config_json(json);
        effect().set_config_json(json["nested"]);
    }

    void get_config_json(JsonObject& json) const override {
        get_parent_config_json(json);
        JsonObject nested = json.createNestedObject("nested");
        effect().get_config_json(nested);
    };

    void trigger(uint32_t now_ms) override { effect().trigger(now_ms); }
    void clear(uint32_t now_ms) override { effect().clear(now_ms); }

protected:
    // To be implemented by the parent class to set configuration
    virtual void set_parent_config_json(const JsonObject& json) {}
    virtual void get_parent_config_json(JsonObject& json) const {}
    virtual String parent_type() const = 0;

private:
    Effect effect_;
};

#include "effects/linear_fade.hh"
#include "effects/cos_blend.hh"
#include "effects/linear_pulse.hh"
#include "effects/sweeping_pulse.hh"
#include "effects/midi.hh"
#include "effects/audio.hh"
#include "effects/blank.hh"

class EffectMap {
public:
    template <typename Effect, typename... Args>
    Effect& add_effect(const String& name, Args&&...args) {
        auto effect_ptr = std::make_unique<Effect>(args...);
        Effect& effect = *effect_ptr;
        effects_[name] = std::move(effect_ptr);
        return effect;
    }

    std::vector<String> names(const String& light_names) const {
        std::vector<String> out;
        out.reserve(effects_.size());
        for (const auto& it : effects_) {
            out.push_back(it.first);
        }
        return out;
    }

    EffectBase* effect(const String& name) const {
        auto it = effects_.find(name);
        if (it == effects_.end()) {
            Serial.print("No effect named '");
            Serial.print(name);
            Serial.println("' found!");
            return nullptr;
        }
        return it->second.get();
    }

    void set_json(const String& name, const JsonObject& object) {
        auto it = effects_.find(name);
        if (it == effects_.end()) {
            Serial.print("No effect named '");
            Serial.print(name);
            Serial.println("' found!");
            return;
        }

        it->second->set_config_json(object);
    }

    DynamicJsonDocument get_json(const String& name) {
        DynamicJsonDocument doc(1024);
        for (const auto& effect : effects_) {
            if (name != "" && effect.first != name) {
                continue;
            }

            JsonObject object = doc.createNestedObject(effect.first);
            object["type"] = effect.second->type();

            JsonObject config = object.createNestedObject("config");
            effect.second->get_config_json(config);
        }

        // Overflows are silent, so make them loud here!
        if (doc.overflowed()) {
            DynamicJsonDocument err(1024);
            err["error"] = "Config for " + std::to_string(effects_.size()) + " effects overflowed the json buffer.";
            return err;
        }

        return doc;
    }

private:
    std::map<String, std::unique_ptr<EffectBase>> effects_;
};

