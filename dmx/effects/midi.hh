#pragma once


#include "MIDI.h"
#include <set>

class MidiManager {
public:
    static MidiManager& instance() {
        static MidiManager* manager_ = new MidiManager();
        return *manager_;
    }

    static void note_off(Channel channel, byte note, byte velocity) {
        instance().note_off(channel, note, velocity);
    }
    static void note_on(Channel channel, byte note, byte velocity) {
        instance().note_on(channel, note, velocity);
    }

private:
    void note_off_impl(Channel channel, byte note, byte velocity) {
        active_channels_.insert(note);
    }
    void note_on_impl(Channel channel, byte note, byte velocity) {
        active_channels_.erase(note);
    }

private:
    std::set<byte> active_channels_;
};

template <typename Effect>
class MidiTrigger : public EffectBase {
public:
    ~MidiTrigger() override = default;

    std::string type() const override {
        std::string type = "MidiTrigger(";
        type += effect_.type();
        return type + ")";
    }

    void set_config_json(const JsonObject& json) override {
        effect_.set_config_json(json);
    }

    void get_config_json(JsonObject& json) const {
        effect_.get_config_json(json);
    };

    void set_values_json(const JsonObject& json) {
        effect_.set_values_json(json);
    }

    void get_values_json(JsonObject& json) const {
        effect_.get_values_json(json);
    };

private:
    Effect effect_;
};

