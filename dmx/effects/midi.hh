#pragma once

#include <set>

class MidiTriggerBase {
public:
    ~MidiTriggerBase() = default;

    virtual void note(byte channel, byte note, byte velocity, bool on) = 0;
};

void note_off(byte channel, byte note, byte velocity);
void note_on(byte channel, byte note, byte velocity);

class MidiManager {
public:
    void setup() {
        usbMIDI.setHandleNoteOff(::note_off);
        usbMIDI.setHandleNoteOn(::note_on);
    }
    void read() {
        usbMIDI.read();
    }

    bool active(byte channel) const {
        return active_channels_.find(channel) != active_channels_.end();
    }
    bool active(std::vector<byte> channels) const {
        for (const auto& channel : channels) {
            if (!active(channel)) {
                return false;
            }
        }
        return true;
    }

    void add_callback(MidiTriggerBase* trigger) {
        callbacks_.push_back(trigger);
    }
    void remove_callback(MidiTriggerBase* ptr) {
        for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
            if (*it == ptr) {
                callbacks_.erase(it);
                return;
            }
        }
    }

    void note_off(byte channel, byte note, byte velocity) {
        active_channels_.insert(note);
        for (auto* ptr : callbacks_) {
            ptr->note(channel, note, velocity, false);
        }
    }
    void note_on(byte channel, byte note, byte velocity) {
        active_channels_.erase(note);
        for (auto* ptr : callbacks_) {
            ptr->note(channel, note, velocity, true);
        }
    }

private:
    std::set<byte> active_channels_;
    std::vector<MidiTriggerBase*> callbacks_;
};

MidiManager midi;

void note_off(byte channel, byte note, byte velocity) {
    midi.note_off(channel, note, velocity);
}
void note_on(byte channel, byte note, byte velocity) {
    midi.note_on(channel, note, velocity);
}

template <typename Effect>
class MidiTrigger final : public EffectBase, public MidiTriggerBase {
public:
    MidiTrigger() { midi.add_callback(static_cast<MidiTriggerBase*>(this)); }
    ~MidiTrigger() override { midi.remove_callback(static_cast<MidiTriggerBase*>(this)); }

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

    void set_note(byte note) {
        note_ = note;
    }

    Effect* effect() {
        return &effect_;
    }

    void trigger(uint32_t now_ms) {
        effect_.trigger(now_ms);
    }
    void clear(uint32_t now_ms) {
        effect_.clear(now_ms);
    }

private:
    void note(byte channel, byte note, byte velocity, bool on) override {
        if (note != note_) {
            return;
        }

        if (on) {
            trigger(now());
        } else {
            // clear(now());
        }
    }

    uint8_t note_ = 0;
    Effect effect_;
};

