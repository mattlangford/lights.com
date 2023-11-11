#pragma once

#include <set>
#include <functional>

class MidiManager {
private:
    using Callback = std::function<void(byte channel, byte note, byte velocity, bool on)>;

    static void dispatch_note_off(byte channel, byte note, byte velocity);
    static void dispatch_note_on(byte channel, byte note, byte velocity);
public:
    void setup() {
        usbMIDI.setHandleNoteOff(dispatch_note_off);
        usbMIDI.setHandleNoteOn(dispatch_note_on);
    }
    void read() {
        usbMIDI.read();
    }

    bool active(byte channel) const { return active_channels_.find(channel) != active_channels_.end(); }
    bool active(std::vector<byte> channels) const {
        for (const auto& channel : channels) {
            if (!active(channel)) {
                return false;
            }
        }
        return true;
    }

    void add_callback(Callback cb) {
        callbacks_.emplace_back(std::move(cb));
    }

    void note_off(byte channel, byte note, byte velocity) {
        active_channels_.insert(note);
        for (auto& cb : callbacks_) {
            cb(channel, note, velocity, false);
        }
    }
    void note_on(byte channel, byte note, byte velocity) {
        active_channels_.erase(note);
        for (auto& cb : callbacks_) {
            cb(channel, note, velocity, true);
        }
    }

private:
    std::set<byte> active_channels_;
    std::vector<Callback> callbacks_;
};

MidiManager midi;

void MidiManager::dispatch_note_off(byte channel, byte note, byte velocity) {
    midi.note_off(channel, note, velocity);
}
void MidiManager::dispatch_note_on(byte channel, byte note, byte velocity) {
    midi.note_on(channel, note, velocity);
}

template <typename Effect>
class MidiTrigger final : public EffectBase {
public:
    MidiTrigger() {
        midi.add_callback([this](byte channel, byte note, byte velocity, bool on){
            on_note(channel, note, velocity, on);
        });
    }

    std::string type() const override {
        std::string type = "MidiTrigger(";
        type += effect_.type();
        return type + ")";
    }

    void set_config_json(const JsonObject& json) override {
        maybe_set(json, "note", note_);
        effect_.set_config_json(json);
    }

    void get_config_json(JsonObject& json) const {
        json["note"] = note_;
        JsonObject effect_config = json.createNestedObject("effect_config");
        effect_.get_config_json(effect_config);
    };

    void set_values_json(const JsonObject& json) {
        effect_.set_values_json(json);
    }

    void get_values_json(JsonObject& json) const {
        effect_.get_values_json(json);
    };

    void set_note(byte note) { note_ = note; }
    Effect* effect() { return &effect_; }
    void trigger(uint32_t now_ms) { effect_.trigger(now_ms); }
    void clear(uint32_t now_ms) { effect_.clear(now_ms); }

private:
    void on_note(byte channel, byte note, byte velocity, bool on) override {
        if (note != note_) {
            return;
        }

        if (on) {
            trigger(now());
        } else {
            clear(now());
        }
    }

    uint8_t note_ = 0;
    Effect effect_;
};

