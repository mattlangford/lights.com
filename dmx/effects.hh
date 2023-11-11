#pragma once

#include "dmx.hh"
#include "util.hh"
#include <ArduinoJson.h>

#include <vector>
#include <map>
#include <memory>

class EffectBase {
public:
    virtual ~EffectBase() = default;

    virtual void set_config_json(const JsonObject& json) = 0;
    virtual void get_config_json(JsonObject& json) const = 0;

    virtual void set_values_json(const JsonObject& json) = 0;
    virtual void get_values_json(JsonObject& json) const = 0;

    virtual String type() const = 0;

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
};

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
            Serial.print(name.c_str());
            Serial.println("' found!");
            return nullptr;
        }
        return it->second.get();
    }

    void set_json(const JsonObject& object) {
        for (auto field : object) {
            auto it = effects_.find(field.key().c_str());
            if (it == effects_.end()) {
                continue;
            }
            auto config = field.value()["config"];
            if (!config.isNull()) it->second->set_config_json(config);
            auto values = field.value()["values"];
            if (!values.isNull()) it->second->set_values_json(values);
        }
    }

    DynamicJsonDocument get_json() {
        DynamicJsonDocument doc(1024);
        for (const auto& effect : effects_) {
            JsonObject object = doc.createNestedObject(effect.first);
            object["type"] = effect.second->type();

            JsonObject config = object.createNestedObject("config");
            JsonObject values = object.createNestedObject("values");

            effect.second->get_config_json(config);
            effect.second->get_values_json(values);
        }
        return doc;
    }

private:
    std::map<String, std::unique_ptr<EffectBase>> effects_;
};

template <typename Effect>
class CompositeEffect : public EffectBase {
public:
    ~CompositeEffect() override = default;

    String type() const override {
        String name = "CompositeEffect(";
        name += effects_.empty() ? Effect().type() : effects_.front()->type();
        name += ")";
        return name;
    }

    Effect& add() {
        effects_.push_back(std::make_unique<Effect>());
        return *effects_.back();
    }

    size_t size() const { return effects_.size(); };

    void set_config_json(const JsonObject& json) override {
        for (auto& effect : effects_) {
            effect->set_config_json(json);
        }
    }

    void get_config_json(JsonObject& json) const {
        if (!effects_.empty()) {
            // Expecting these all to the same, just pick the first
            effects_.front()->get_config_json(json);
        }
    };

    void set_values_json(const JsonObject& json) {
        for (auto& effect : effects_) {
            effect->set_values_json(json);
        }
    }

    void get_values_json(JsonObject& json) const {
        if (!effects_.empty()) {
            // Expecting these all to the same, just pick the first
            effects_.front()->get_values_json(json);
        }
    };

    void trigger(uint32_t now_ms) {
        if (effects_.empty()) return;
        // Trigger a random effect
        // TODO: Add a config for this.
        effects_[random(effects_.size())]->trigger(now_ms);
    }
    void clear(uint32_t now_ms) {
        for (auto& effect : effects_) {
            effect->clear(now_ms);
        }
    }


private:
    std::vector<std::unique_ptr<Effect>> effects_;
};

template <typename Effect>
class RgbEffect final : public EffectBase {
public:
    RgbEffect() = default;
    ~RgbEffect() override = default;

public:
    String type() const override { return "RGB(" + r_.type() + ")";; }

    Effect* red() { return &r_; }
    Effect* green() { return &g_; }
    Effect* blue() { return &b_; }

    template <typename Config>
    void set_config(const Config& config) {
        r_.set_config(config);
        g_.set_config(config);
        b_.set_config(config);
    }

    void set_max_values(uint8_t red, uint8_t green, uint8_t blue) {
        r_.set_values(0, red);
        g_.set_values(0, green);
        b_.set_values(0, blue);
    }

    void set_config_json(const JsonObject& json) override {
        r_.set_config_json(json["red"]);
        g_.set_config_json(json["green"]);
        b_.set_config_json(json["blue"]);
    }

    void get_config_json(JsonObject& json) const {
        JsonObject red = json.createNestedObject("red");
        r_.get_config_json(red);
        JsonObject green = json.createNestedObject("green");
        g_.get_config_json(green);
        JsonObject blue = json.createNestedObject("blue");
        b_.get_config_json(blue);
    };

    void set_values_json(const JsonObject& json) {
        r_.set_values_json(json["red"]);
        g_.set_values_json(json["green"]);
        b_.set_values_json(json["blue"]);
    }

    void get_values_json(JsonObject& json) const {
        JsonObject red = json.createNestedObject("red");
        r_.get_values_json(red);
        JsonObject green = json.createNestedObject("green");
        g_.get_values_json(green);
        JsonObject blue = json.createNestedObject("blue");
        b_.get_values_json(blue);
    };

    void trigger(uint32_t now_ms) {
        r_.trigger(now_ms);
        g_.trigger(now_ms);
        b_.trigger(now_ms);
    }
    void clear(uint32_t now_ms) {
        r_.clear(now_ms);
        g_.clear(now_ms);
        b_.clear(now_ms);
    }

private:
    Effect r_;
    Effect g_;
    Effect b_;
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

#include "effects/linear_fade.hh"
#include "effects/cos_blend.hh"
#include "effects/linear_pulse.hh"
#include "effects/sweeping_pulse.hh"
#include "effects/midi.hh"
#include "effects/audio.hh"

