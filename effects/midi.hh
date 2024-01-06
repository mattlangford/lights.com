#pragma once

#include <set>
#include <functional>
#include <unordered_map>

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
            return C0_BASE + OCTIVE_SIZE * octive + offset + sharp ? 1 : 0;
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

MidiManager midi;

void MidiManager::dispatch_note_off(byte channel, byte note, byte velocity) {
    midi.note_off(channel, note, velocity);
}
void MidiManager::dispatch_note_on(byte channel, byte note, byte velocity) {
    midi.note_on(channel, note, velocity);
}

///
/// @brief Triggers an effect when the given MIDI note is provided.
///
template <typename Effect>
class MidiTrigger final : public NestedEffect<Effect> {
public:
    MidiTrigger() {
        midi.add_callback([this](byte channel, byte note, byte velocity, bool on){
            on_note(channel, note, velocity, on);
        });
    }

    void set_note(byte note) { note_ = note; }

protected:
    String parent_type() const override { return "MidiTrigger"; }
    void set_parent_config_json(const JsonObject& json) override { this->maybe_set(json, "note", note_); }
    void get_parent_config_json(JsonObject& json) const override { json["note"] = note_; };

private:
    void on_note(byte channel, byte note, byte velocity, bool on) override {
        if (note != note_) {
            return;
        }

        if (on) {
            this->trigger(this->now(), static_cast<float>(velocity) / 255.0);
        } else {
            this->clear(this->now());
        }
    }

    uint8_t note_ = 0;
};

/// @brief Similar to MidiTrigger, but aggregates many instances of Effect 
template <typename Effect>
class MidiMap final : public EffectBase {
public:
    MidiMap() {
        midi.add_callback([this](byte channel, byte note, byte velocity, bool on){
            on_note(channel, note, velocity, on);
        });
    }

    String type() const override {
        String name = "MidiMap(";
        // Awkward, but just hope they don't call this before populating
        name += effects_.empty() ? "?" : effects_.begin()->type();
        name += ")";
        return name;
    }

    void set_config_json(const JsonObject& json) override {
        for (auto& effect_it : effects_) {
            effect_it->second.set_config_json(json);
        }
    }

    void get_config_json(JsonObject& json) const override {
        if (!effects_.empty()) {
            effects_.begin()->get_config_json(json);
        }
    }

    void trigger(uint32_t now_ms, float scale) {}
    void clear(uint32_t now_ms) {}

    Effect& add() {
        effects_.push_back(std::make_unique<Effect>());
        return *effects_.back();
    }

private:
    void on_note(byte channel, byte note, byte velocity, bool on) override {
        auto it = effects_.find(note);
        if (it == effects_.end()) {
            return;
        }
        if (on) {
            it->second->trigger(this->now(), static_cast<float>(velocity) / 255.0);
        } else {
            it->second->clear(this->now());
        }
    }

private:
    std::unordered_map<uint8_t, std::unique_ptr<Effect>> effects_;
};