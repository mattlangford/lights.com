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
            this->trigger(this->now());
        } else {
            this->clear(this->now());
        }
    }

    uint8_t note_ = 0;
};

