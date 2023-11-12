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
