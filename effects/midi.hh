#pragma once

#include <set>
#include <functional>
#include <unordered_map>

#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, SerialMIDI);

class MidiManager {
private:
    using Callback = std::function<void(byte channel, byte note, byte velocity, bool on)>;

    static void dispatch_note_off(byte channel, byte note, byte velocity);
    static void dispatch_note_on(byte channel, byte note, byte velocity);
public:
    // void setup() {
    //     usbMIDI.setHandleNoteOff(dispatch_note_off);
    //     usbMIDI.setHandleNoteOn(dispatch_note_on);
    // }
    // void read() {
    //     usbMIDI.read();
    // }

    void setup() {
        SerialMIDI.begin(10);
    }

    void read() {
        if (SerialMIDI.read()) { 
            switch (SerialMIDI.getType()) {
            case midi::NoteOn: {
                note_on(SerialMIDI.getChannel(), SerialMIDI.getData1(), SerialMIDI.getData2());
                break;
            }
            case midi::NoteOff: {
                note_off(SerialMIDI.getChannel(), SerialMIDI.getData1(), SerialMIDI.getData2());
                break;
            }
            default:
                break;
            }
        }
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

public:
    struct MidiNote {
        char base = 'A';
        byte octive = 0;
        bool sharp = false;

        static constexpr byte C0_BASE = 12;
        static constexpr byte OCTIVE_SIZE = 12;
        byte note() const {
            byte offset = 0;
            switch (base) {
                case 'C': case 'c': offset = 0; break;
                case 'D': case 'd': offset = 2; break;
                case 'E': case 'e': offset = 4; break;
                case 'F': case 'f': offset = 5; break;
                case 'G': case 'g': offset = 7; break;
                case 'A': case 'a': offset = 9; break;
                case 'B': case 'b': offset = 11; break;
                default: break;
            }
            return C0_BASE + OCTIVE_SIZE * octive + offset + (sharp ? 1 : 0);
        }

        static MidiNote from_note(byte note) {
            MidiNote output;

            note -= C0_BASE;
            output.octive = note / OCTIVE_SIZE;

            switch (note % OCTIVE_SIZE) {
                case 11: output.base = 'B'; break;
                case 10: output.sharp = true;
                case 9: output.base = 'A'; break;
                case 8: output.sharp = true;
                case 7: output.base = 'G'; break;
                case 6: output.sharp = true;
                case 5: output.base = 'F'; break;
                case 4: output.base = 'E'; break;
                case 3: output.sharp = true;
                case 2: output.base = 'D'; break;
                case 1: output.sharp = true;
                case 0: output.base = 'C'; break;
                default: break;
            }
            return output;
        }
    };

private:
    std::set<byte> active_channels_;
    std::vector<Callback> callbacks_;
};

MidiManager midi_manager;

void MidiManager::dispatch_note_off(byte channel, byte note, byte velocity) {
    midi_manager.note_off(channel, note, velocity);
}
void MidiManager::dispatch_note_on(byte channel, byte note, byte velocity) {
    midi_manager.note_on(channel, note, velocity);
}

///
/// @brief Triggers an effect when the given MIDI note is provided.
///
template <typename Effect>
class MidiTrigger final : public NestedEffect<Effect> {
public:
    explicit MidiTrigger(byte note) : note_(note) {
        midi_manager.add_callback([this](byte channel, byte note, byte velocity, bool on){
            on_note(channel, note, velocity, on);
        });
    }
    MidiTrigger(char base, byte octive, bool sharp = false) :
        MidiTrigger(MidiManager::MidiNote{base, octive, sharp}.note()) { }
    MidiTrigger() : MidiTrigger(-1, true) { }

    ~MidiTrigger() override = default;

    void set_note(byte note) { note_ = note; }
    void set_any_node(bool enable=true) { any_ = enable; }
    void set_enabled(bool enabled) { enabled_ = enabled; }
    void set_every_n(uint8_t every_n) { every_n_ = every_n; }

protected:
    String parent_type() const override { return "MidiTrigger"; }

    SetConfigResult set_parent_config_json(const JsonObject& json) override {
        SetConfigResult result = SetConfigResult::no_values_set();
        result.maybe_set(json, "note", note_);
        result.maybe_set(json, "any", any_);
        if (result.maybe_set(json, "enabled", enabled_)) {
            if (enabled_) {
                this->trigger(millis());
            } else {
                this->clear(millis());
            }
        }
        result.maybe_set(json, "ignore_clear", ignore_clear_);
        result.maybe_set(json, "every_n", every_n_);
        return result;
    }
    void get_parent_config_json(JsonObject& json) const override {
        json["note"] = note_;
        json["any"] = any_;
        json["enabled"] = enabled_;
        json["ignore_clear"] = ignore_clear_;
        json["every_n"] = every_n_;
    };

private:
    void on_note(byte channel, byte note, byte velocity, bool on) {
        if (!enabled_) {
            return;
        }
        if (!any_ && note != note_) {
            return;
        }

        if (on) {
            // How do we do this for the trigger too?
            if (++n_ < every_n_) { return; }
            n_ = 0;
            Serial.print("TRIGGER ");
            Serial.println(this->type());
            this->trigger(this->now());
        } else if (!ignore_clear_) {
            Serial.print("CLEAR ");
            Serial.println(this->type());
            this->clear(this->now());
        }
    }

    uint8_t note_ = 0;
    bool any_ = false;
    bool enabled_ = true;
    bool ignore_clear_ = true;

    uint8_t every_n_ = 1;
    uint8_t n_ = 0;
};

/// @brief Similar to MidiTrigger, but aggregates many instances of Effect 
template <typename Effect>
class MidiMap final : public CompositeEffect<Effect> {
public:
    MidiMap(byte channel=-1) : channel_(channel) {
        midi_manager.add_callback([this](byte channel, byte note, byte velocity, bool on){
            on_note(channel, note, velocity, on);
        });
    }

    String type() const override { return "MidiMap(" + this->subtype() + ")"; }

    Effect& add_effect_for_note(uint8_t note) {
        note_to_effect_[note] = this->size();
        return this->add();
    }

    Effect& add_effect_for_note(char base, byte octive, bool sharp = false) {
        note_to_effect_[MidiManager::MidiNote{base, octive, sharp}.note()] = this->size();
        return this->add();
    }

    void set_channel(byte channel) {
        channel_ = channel;
    }

private:
    // So that callers have to go through our public accessors
    using CompositeEffect<Effect>::add;
    using CompositeEffect<Effect>::effect;

    void on_note(byte channel, byte note, byte velocity, bool on) {
        if (channel_ != -1 && channel != channel_) {
            return;
        }

        Effect* effect = effect_for_note(note);
        if (effect == nullptr) {
            return;
        }

        if (on) {
            effect->trigger(this->now());
        } else {
            // effect->clear(this->now());
        }
    }

private:
    Effect* effect_for_note(uint8_t note) {
        auto it = note_to_effect_.find(note);
        if (it == note_to_effect_.end()) {
            return nullptr;
        }

        return this->effect(it->second);
    }

    // -1 implies unuset.
    byte channel_ = -1;

    // Maps the note number to underlying effect number.
    std::unordered_map<uint8_t, size_t> note_to_effect_;
};