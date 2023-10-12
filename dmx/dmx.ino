#include "dmx.hh"
#include "lights.hh"

DMXController* controller;

void setup() {
    controller = new DMXController(13, 9);

    Serial.begin(115200);

    FadeChannel& channel = controller->channel(1);
    channel.goal = 255;
    channel.remaining_us = 10'000'000;
}

void loop() {
    controller->write_frame();
    delay(100);
}
