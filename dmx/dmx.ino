#include "dmx.hh"
//#include "lights.hh"

DMXController* controller;

//WashLightBar52* light;

uint8_t light_from_note(byte note) {
  switch (note) {
    case 52: return 0;
    case 53: return 1;
    case 55: return 2;
    case 57: return 3;
    case 59: return 4;
    case 60: return 5;
    case 62: return 6;
    case 64: return 7;
    case 65: return 8;
    case 67: return 9;
    case 69: return 10;
    case 71: return 11;
    case 72: return 12;
    case 74: return 13;
    case 76: return 14;
    case 77: return 15;
    default: return 255;
  }
}

// constexpr uint8_t MIN_NOTE = 72;
// constexpr uint8_t MAX_NOTE = MIN_NOTE + 16;
// 
// void note_on(byte channel, byte note, byte velocity) {
//     uint8_t index = light_from_note(note);
//     if (index > WashLightBar52::NUM_LIGHTS) return;
//     light->rgb[index].set_goal(velocity, 2 * velocity, 0, 0);
// }
// 
// void note_off(byte channel, byte note, byte velocity) {
//     uint8_t index = light_from_note(note);
//     if (index > WashLightBar52::NUM_LIGHTS) return;
//     light->rgb[index].set_goal(0, 0, 0, 200);
// }

struct Fade {
    uint32_t end_ms = 0;
    float slope = 0.0;
    uint8_t end_value = 0;

    void set_goal(uint8_t new_goal, uint32_t duration_ms) {
        uint32_t now_ms = millis();

        const uint8_t current = value(now_ms);
        slope = (static_cast<float>(new_goal) - static_cast<float>(current)) / static_cast<float>(duration_ms);

        end_value = new_goal;
        end_ms = now_ms + duration_ms;
    }

    bool done() const {
        return millis() > end_ms;
    }

    uint8_t value(uint32_t now_ms) const {
        if (now_ms >= end_ms) {
            return end_value;
        }

        const uint32_t remaining = end_ms - now_ms;
        const float value = end_value - slope * remaining;
        Serial.print("remaining: ");
        Serial.print(remaining);
        Serial.print(" value ");
        Serial.println(value, 7);

        if (value < 0) {
            return 0;
        } else if (value > 255) {
            return 255;
        }
        return value;
    }

    static uint8_t callback(uint32_t now_ms, void* context) {
        return cast<Fade>(context).value(now_ms);
    }
};

Fade* f;

void setup() {
    controller = new DMXController(41, 40);

    f = new Fade();
    controller->add_callback(1, ChannelCallback(&Fade::callback, f));

    Serial.begin(115200);

    // usbMIDI.setHandleNoteOn(note_on);
    // usbMIDI.setHandleNoteOff(note_off);

    // uint8_t address = 1;
    // light = new WashLightBar52(address, *controller);
    // Serial.print("Final address: ");
    // Serial.println(address);

    // light->brightness.goal = 255;

    // controller->set_max_channel(53);
}

void loop() {
    // usbMIDI.read();
    controller->write_frame();

    // for (Rgb& rgb : light->rgb) {
    //   if (rgb.r.remaining_ms == 0) {
    //     rgb.set_goal(0, 0, 0, 3000);
    //   }
    // }
}

