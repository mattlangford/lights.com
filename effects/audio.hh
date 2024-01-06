#pragma once

#include <Audio.h>

class AudioManager {
public:
    using Callback = std::function<void(float, float)>;

    AudioInputUSB        usb_1;
    AudioAnalyzePeak     peak_l;
    AudioAnalyzePeak     peak_r;
    AudioAnalyzeFFT1024  fft;
    AudioConnection      patchCord1{usb_1, 0, peak_l, 0};
    AudioConnection      patchCord2{usb_1, 1, peak_r, 0};
    AudioConnection      patchCord3{usb_1, fft};

public:
    void setup() {
        AudioMemory(25);
    }
    void read() {
        float left = peak_l.available() ? peak_l.read() : -1.0;
        float right = peak_r.available() ? peak_r.read() : -1.0;
        for (auto& cb : callbacks_) {
            cb(left, right);
        }
    }

    void add_callback(Callback cb) { callbacks_.emplace_back(std::move(cb)); }

private:
    std::vector<Callback> callbacks_;
};

AudioManager audio;

struct AudioLevelConfig {
    float threshold = 0.0;
    uint8_t port = 0;
};

template <typename Effect>
class AudioTrigger final : public NestedEffect<Effect> {
public:
    AudioTrigger() { audio.add_callback([this](float left, float right){ callback(left, right); }); }
    ~AudioTrigger() override {}

    void set_config(AudioLevelConfig config) {
        config_ = std::move(config);
    }

protected:
    void set_parent_config_json(const JsonObject& json) override {
        this->maybe_set(json, "threshold", config_.threshold);
        this->maybe_set(json, "port", config_.port);
    }

    void get_parent_config_json(JsonObject& json) const override {
        json["threshold"] = config_.threshold;
        json["port"] = config_.port;
    };

    String parent_type() const override { return "AudioTrigger"; }

private:
    void callback(float left, float right) {
        float value = config_.port == 0 ? left : right;
        bool state = value > config_.threshold;
        if (state != last_state_) {
            if (state) {
                this->trigger(this->now());
            } else {
                this->clear(this->now());
            }
        }
        last_state_ = state;
    }

    bool last_state_ = false;
    AudioLevelConfig config_;
};

struct AudioFrequencyLevelConfig {
    int min_bin = 0;
    int max_bin = 0;
    float fft_gain = 7.0;
};

class AudioFrequencyValues final : public SingleChannelEffect {
public:
    ~AudioFrequencyValues() override {}

    void set_config(AudioFrequencyLevelConfig config) {
        config_ = std::move(config);
    }

    String type() const override { return "AudioFrequencyValues"; }

    void set_config_json(const JsonObject& json) override {
        SingleChannelEffect::set_config_json(json);
        maybe_set(json, "min_bin", config_.min_bin);
        maybe_set(json, "max_bin", config_.max_bin);
        maybe_set(json, "fft_gain", config_.fft_gain);
    }
    void get_config_json(JsonObject& json) const override {
        SingleChannelEffect::get_config_json(json);
        json["min_bin"] = config_.min_bin;
        json["max_bin"] = config_.max_bin;
        json["fft_gain"] = config_.fft_gain;
    }

protected:
    float level(uint32_t now_ms) {
        return config_.fft_gain * audio.fft.read(config_.min_bin, config_.max_bin);
    }

private:
    AudioFrequencyLevelConfig config_;
};

class AudioMeter : public EffectBase {
public:
    using Effect = AudioTrigger<RgbEffect<LinearFade>>;

public:
    AudioMeter(size_t count, uint8_t port) : effects_(count) {
        size_t red_index = 0.2 * count;
        size_t yellow_index = 0.5 * count;

        for (size_t i = 0; i < count; ++i) {
            AudioLevelConfig config;
            config.port = port;
            config.threshold = 0.8 * std::pow(10, -static_cast<float>(i) / (count - 1));
            effects_[i].set_config(config);

            const LinearFadeConfig fade_config{.trigger_dt_ms = 0, .clear_dt_ms = 0 };
            auto& light = rgb(i);
            light.set_config(fade_config);
            light.red().set_values(0, 0);
            light.green().set_values(0, 0);
            light.blue().set_values(0, 0);

            if (i < red_index) {
                light.red().set_values(10, 128);
            } else if (i < yellow_index) {
                light.red().set_values(10, 100);
                light.green().set_values(10, 100);
            } else {
                light.green().set_values(10, 75);
            }
        }
    }
    ~AudioMeter() override = default;

    String type() const { return "AudioMeter"; }

    void set_config_json(const JsonObject& json) override {};
    void get_config_json(JsonObject& json) const override {};

    RgbEffect<LinearFade>& rgb(size_t i) { return effects_[i].effect(); }

private:
    std::vector<Effect> effects_;
};