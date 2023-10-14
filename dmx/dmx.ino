#include "dmx.hh"
#include "lights.hh"

DMXController* controller;

WashLightBar52* light;

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



void setup() {
    controller = new DMXController(41, 40);

    light = new WashLightBar52(1, *controller);
    light->brightness.set_goal(255, 10'000);

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

float phase = 0.0;

void loop() {
    light->brightness.set_goal(255);
    for (uint8_t i = 0; i < WashLightBar52::NUM_LIGHTS; ++i) {
        float hue = 0.5 * sin(phase + 0.1 * static_cast<float>(i)) + 0.5;
        light->rgb[i].set_goal_hsv(255 * hue, 255, 255);
    }
    phase += 0.01;

    // usbMIDI.read();
    controller->write_frame();

    // for (Rgb& rgb : light->rgb) {
    //   if (rgb.r.remaining_ms == 0) {
    //     rgb.set_goal(0, 0, 0, 3000);
    //   }
    // }
}

