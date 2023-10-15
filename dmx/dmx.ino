#include "dmx.hh"
#include "lights.hh"

#include <MIDIUSB.h>

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

DMXController* controller;
WashLightBar52* light1;
WashLightBar52* light2;

uint8_t current = 0;

void note_on(byte channel, byte note, byte velocity) {
    Serial.print("Note ch=");
    Serial.print((int)channel);
    Serial.print(" note=");
    Serial.print((int)note);
    Serial.print(" velocity=");
    Serial.println((int)velocity);
    return;
    if (note == 72) {
      light1->rgb[current].set_goal(0, 100, 255, 50);
      light2->rgb[current].set_goal(0, 100, 255, 100);
    } else if (note == 74) {
      light1->rgb[current].set_goal(100, 100, 0, 50);
    } else if (note == 76) {
      light1->rgb[current].set_goal(255, 100, 0, 50);
      light2->rgb[current].set_goal(255, 100, 0, 100);
    } else if (note == 77) {
      light2->rgb[current].set_goal(100, 100, 0, 50);
    }
}

void note_off(byte channel, byte note, byte velocity) {
    if (note == 72) {
      light1->set_goal(0, 0, 0, 300);
      light2->set_goal(0, 0, 0, 300);
    } else if (note == 74) {
      light1->set_goal(0, 0, 0, 100);
    } else if (note == 76) {
      light1->set_goal(0, 0, 0, 300);
      light2->set_goal(0, 0, 0, 300);
    } else if (note == 77) {
      light2->set_goal(0, 0, 0, 50);
    }
}

void setup() {
    controller = new DMXController(41, 40);

    light1 = new WashLightBar52(1, *controller);
    light1->brightness().set_goal(255);
    light1->set_goal(200, 200, 200);

    light2 = new WashLightBar52(54, *controller);
    light2->brightness().set_goal(255);
    light2->set_goal(200, 200, 200);

    Serial.begin(115200);

    usbMIDI.setHandleNoteOn(note_on);
    usbMIDI.setHandleNoteOff(note_off);
}

template <typename Channel>
void set_color(RgbChannel<Channel>& channel) {
    uint8_t h = random(150, 190);
    uint8_t s = random(200, 255);
    uint8_t v = random(255, 255);
    uint32_t t = random(5000, 10000);
    channel.set_goal_hsv(h, s, v, t);
}

uint32_t last_ms = 0;
void loop() {
    uint32_t now_ms = millis();
    if (now_ms > 1000 + last_ms) {
        Serial.println("Hello1");
        last_ms = now_ms;
    }

    usbMIDI.read();
    controller->write_frame();
}

