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

    virtual std::string type() const = 0;

protected:
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
    Effect& add_effect(const std::string& name, Args&&...args) {
        auto effect_ptr = std::make_unique<Effect>(args...);
        Effect& effect = *effect_ptr;
        effects_[name] = std::move(effect_ptr);
        return effect;
    }

    std::vector<std::string> names(const std::string& light_names) const {
        std::vector<std::string> out;
        out.reserve(effects_.size());
        for (const auto& it : effects_) {
            out.push_back(it.first);
        }
        return out;
    }

    EffectBase* effect(const std::string& name) const {
        auto it = effects_.find(name);
        return it == effects_.end() ? nullptr : it->second.get();
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
    std::map<std::string, std::unique_ptr<EffectBase>> effects_;
};

///
///
///

struct LinearFadeConfig {
    uint32_t trigger_dt_ms = 1000;
    uint32_t clear_dt_ms = 1000;
};

class LinearFade final : public ChannelEffect, public EffectBase {
public:
    ~LinearFade() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        if (now_ms < start_ms_) { return clip(value + start_value_); }
        if (now_ms >= end_ms_) { return clip(value + end_value_); }

        const float fade_ratio = (static_cast<float>(now_ms) - start_ms_) / (end_ms_ - start_ms_);
        const float diff = static_cast<float>(start_value_) - end_value_;
        return clip(value + start_value_ + fade_ratio * diff);
    }

    void trigger(uint32_t now_ms) override {
        set(now_ms, config_.trigger_dt_ms, min_, max_);
    }
    void clear(uint32_t now_ms) override {
        set(now_ms, config_.clear_dt_ms, max_, min_);
    }

    void set_config(const LinearFadeConfig& config) { config_ = config; }
    void set_values(uint8_t min, uint8_t max) { min_ = min; max_ = max; }

public:
    std::string type() const override { return "LinearFade"; }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "trigger_dt_ms", config_.trigger_dt_ms);
        maybe_set(json, "clear_dt_ms", config_.clear_dt_ms);
    }

    void get_config_json(JsonObject& json) const override {
        json["trigger_dt_ms"] = config_.trigger_dt_ms;
        json["clear_dt_ms"] = config_.clear_dt_ms;
    }

    void set_values_json(const JsonObject& json) override {
        maybe_set(json, "min", config_.trigger_dt_ms);
        maybe_set(json, "max", config_.clear_dt_ms);
    }

    void get_values_json(JsonObject& json) const override {
        json["min"] = min_;
        json["max"] = max_;
    }

private:
    void set(uint32_t now_ms, uint32_t duration_ms, uint8_t start, uint8_t end) {
        start_ms_ = now_ms;
        end_ms_ = now_ms + duration_ms;
        start_value_ = start;
        end_value_ = end;
    }

    LinearFadeConfig config_;
    uint8_t min_ = 0;
    uint8_t max_ = 255;

    uint32_t start_ms_ = 0;
    uint32_t end_ms_ = 0;
    uint8_t start_value_ = 0;
    uint8_t end_value_ = 0;
};

struct CosBlendConfig {
    uint8_t depth = 1;
    float min_freq = 10;
    float max_freq = 1000;
    float passthrough = 0.0;
};

class CosBlend final : public ChannelEffect, public EffectBase {
public:
    ~CosBlend() override = default;

    CosBlend() { update_waves(); }

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        const float dt = static_cast<float>(now_ms - last_time_ms_) / 1000;
        last_time_ms_ = now_ms;

        float new_value = config_.passthrough * value;
        for (auto& wave : waves_) {
            wave.phase = fmod(wave.phase + wave.freq * dt, 2.0f * M_PI);
            new_value += generate(wave.phase, min_, max_);
        }

        return clip(new_value / config_.depth);
    }

    void set_config(const CosBlendConfig& config) {
        config_ = config;
        update_waves();
    }
    void set_values(uint8_t min, uint8_t max) {
        min_ = static_cast<float>(min);
        max_ = static_cast<float>(max);
    }

public:
    std::string type() const override { return "CosBlend"; }

    void set_config_json(const JsonObject& json) override {
        bool updated = false;
        updated |= maybe_set(json, "depth", config_.depth);
        updated |= maybe_set(json, "min_freq", config_.min_freq);
        updated |= maybe_set(json, "max_freq", config_.max_freq);
        updated |= maybe_set(json, "passthrough", config_.passthrough);

        if (updated) {
            update_waves();
        }
    }

    void get_config_json(JsonObject& json) const override {
        json["depth"] = config_.depth;
        json["min_freq"] = config_.min_freq;
        json["max_freq"] = config_.max_freq;
        json["passthrough"] = config_.passthrough;
    }

    void set_values_json(const JsonObject& json) override {
        uint8_t value = 0;
        if (maybe_set(json, "min", value)) {
            min_ = static_cast<float>(value);
        }
        if (maybe_set(json, "max", value)) {
            max_ = static_cast<float>(value);
        }
    }

    void get_values_json(JsonObject& json) const override {
        json["min"] = static_cast<uint8_t>(min_);
        json["max"] = static_cast<uint8_t>(max_);
    }

private:
    static float generate(float phase, float min, float max) {
        return (0.5 * (cosf(phase) + 1)) * (max - min) + min;
    }

    void update_waves() {
        waves_.resize(config_.depth);

        for (size_t i = 0; i < config_.depth; ++i) {
            waves_[i].freq = random(config_.min_freq, config_.max_freq);
            waves_[i].phase = random(0, 2 * M_PI);
        }
    }

    CosBlendConfig config_;

    struct Wave {
        float phase = 0.0;
        float freq = 0.0;
    };
    std::vector<Wave> waves_;

    uint32_t last_time_ms_ = 0;
    float min_ = 0.0;
    float max_ = 255.0;
};


///
///
///

template <typename Effect>
class CompositeEffect : public EffectBase {
public:
    ~CompositeEffect() override = default;

    std::string type() const override {
        std::string name = "CompositeEffect(";
        name += effects_.empty() ? Effect().type() : effects_.front()->type();
        name += ")";
        return name;
    }

    Effect& add() {
        effects_.push_back(std::make_unique<Effect>());
        return *effects_.back();
    }

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

private:
    std::vector<std::unique_ptr<Effect>> effects_;
};

void hsv_to_rgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);

template <typename Effect>
class RgbEffect final : public EffectBase {
public:
    RgbEffect() = default;
    ~RgbEffect() override = default;

public:
    std::string type() const override { return "RGB(" + r_.type() + ")";; }

    Effect* red() { return &r_; }
    Effect* green() { return &g_; }
    Effect* blue() { return &b_; }

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
