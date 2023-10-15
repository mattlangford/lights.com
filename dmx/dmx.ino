#include "dmx.hh"
#include "lights.hh"

#include <MIDIUSB.h>

DMXController* controller;
WashLightBar52* light1;
WashLightBar52* light2;

void note_on(byte channel, byte note, byte velocity) {
    Serial.print("Note ch=");
    Serial.print((int)channel);
    Serial.print(" note=");
    Serial.print((int)note);
    Serial.print(" velocity=");
    Serial.println((int)velocity);
}

void note_off(byte channel, byte note, byte velocity) {
}

void setup() {
    Serial.begin(115200);

    controller = new DMXController(41, 40);

    light1 = new WashLightBar52(1, *controller);
    light1->brightness().set_goal(255);
    light1->set_goal(200, 200, 200);

    light2 = new WashLightBar52(54, *controller);
    light2->brightness().set_goal(255);
    light2->set_goal(200, 200, 200);

    usbMIDI.setHandleNoteOn(note_on);
    usbMIDI.setHandleNoteOff(note_off);
}

void loop() {
    usbMIDI.read();
    controller->write_frame();
}

