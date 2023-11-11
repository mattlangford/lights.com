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
    void setup() {
        filter_l.frequency(2000);
        filter_r.frequency(2000);
        AudioMemory(6);
    }
    void read() {
        value_l_ = rms_l.available() ? rms_l.read() : -1.0;
        value_r_ = rms_r.available() ? rms_r.read() : -1.0;
    }
    float value(uint8_t port=0) const {
        switch (port) {
            case 0: return value_l_;
            case 1: return value_r_;
        }
        return -1.0;
    }

private:
    float value_l_ = -1.0;
    float value_r_ = -1.0;
};

AudioManager audio;

struct AudioLevelConfig {
    float threshold = 0.0;
    uint8_t port = 0;
};

class AudioLevel final : public ChannelEffect, public EffectBase {
public:
    ~AudioLevel() override {}

public:
    uint8_t process(uint8_t value, uint32_t now_ms) override {
        if (audio.value(config_.port) >= config_.threshold) {
            change_ = now_ms;
            return max_;
        }

        return now_ms > (change_ + 20) ? min_ : max_;
    }

    void set_values(uint8_t min, uint8_t max) { min_ = min; max_ = max; }

public:
    std::string type() const override { return "LinearFade"; }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "threshold", config_.threshold);
        maybe_set(json, "port", config_.port);
    }

    void get_config_json(JsonObject& json) const override {
        json["threshold"] = config_.threshold;
        json["port"] = config_.port;
    }

    void set_values_json(const JsonObject& json) override {
        maybe_set(json, "min", min_);
        maybe_set(json, "max", max_);
    }

    void get_values_json(JsonObject& json) const override {
        json["min"] = min_;
        json["max"] = max_;
    }

    void set_config(AudioLevelConfig config) {
        config_ = std::move(config);
    }

private:
    uint32_t change_ = 0;

    AudioLevelConfig config_;

    uint8_t min_ = 0;
    uint8_t max_ = 255;
};
