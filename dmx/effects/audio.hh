#pragma once

#include <Audio.h>

AudioInputUSB            usb_1;           //xy=193,468
AudioFilterStateVariable filter_l;        //xy=377,401
AudioFilterStateVariable filter_r;        //xy=370,487
AudioAnalyzeRMS          rms_l;           //xy=575,387
AudioAnalyzeRMS          rms_r;           //xy=596,468
AudioConnection          patchCord1(usb_1, 0, filter_l, 0);
AudioConnection          patchCord2(usb_1, 1, filter_r, 0);
AudioConnection          patchCord3(filter_r, 0, rms_r, 0);
AudioConnection          patchCord4(filter_l, 0, rms_l, 0);


class AudioManager {
public:
    using Callback = std::function<void(float, float)>;

public:
    void setup() {
        filter_l.frequency(2000);
        filter_r.frequency(2000);
        AudioMemory(6);
    }
    void read() {
        float left = rms_l.available() ? rms_l.read() : -1.0;
        float right = rms_r.available() ? rms_r.read() : -1.0;
        for (auto& cb : callbacks_) {
            cb(left, right);
        }
    }

    void add_callback(Callback cb) {
        callbacks_.emplace_back(std::move(cb));
    }

private:
    float value_l_ = -1.0;
    float value_r_ = -1.0;

    std::vector<Callback> callbacks_;
};

AudioManager audio;

struct AudioLevelConfig {
    float threshold = 0.0;
    uint8_t port = 0;
};

template <typename Effect>
class AudioTrigger final : public EffectBase {
public:
    AudioTrigger() {
        audio.add_callback([this](float left, float right){ callback(left, right); });
    }
    ~AudioTrigger() override {}

public:
    String type() const override {
        String type = "MidiTrigger(";
        type += effect_.type();
        return type + ")";
    }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "threshold", config_.threshold);
        maybe_set(json, "port", config_.port);
        effect_.set_config_json(json["effect_config"]);
    }

    void get_config_json(JsonObject& json) const override {
        json["threshold"] = config_.threshold;
        json["port"] = config_.port;
        JsonObject effect_config = json.createNestedObject("effect_config");
        effect_.get_config_json(effect_config);
    };

    void set_config(AudioLevelConfig config) {
        config_ = std::move(config);
    }

    void set_values_json(const JsonObject& json) override { effect_.set_values_json(json); }
    void get_values_json(JsonObject& json) const override { effect_.get_values_json(json); };

    Effect& effect() { return effect_; }

    void trigger(uint32_t now) override { effect_.trigger(now); }
    void clear(uint32_t now) override { effect_.clear(now); }

private:
    void callback(float left, float right) {
        float value = config_.port == 0 ? left : right;
        bool state = value > config_.threshold;

        if (state != last_state_) {
            if (state) {
                trigger(now());
            } else {
                clear(now());
            }
        }
        last_state_ = state;
    }

    bool last_state_ = false;
    AudioLevelConfig config_;

    Effect effect_;
};
