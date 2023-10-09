#include "dmx.hh"


uint8_t lights_aurora[38] = {
    255, 255, 15, 70, 0, 0, 0,
    255, 50, 90, 200, 0, 0, 0,
    255, 255, 30, 50, 0, 0, 0,
    255, 30, 50, 200, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t lights_sunset[38] = {
    255, 150, 100, 30, 0, 0, 0,
    255, 150, 50, 75, 0, 0, 0,
    255, 150, 50, 75, 0, 0, 0,
    255, 150, 100, 30, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t lights_halloween[38] = {
    0,  255, 50, 10, 0, 0, 0,
    0, 128, 0, 128, 0, 0, 0,
    0, 255, 50, 10, 0, 0, 0,
    0, 128, 0, 128, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

DMXController* controller;

void setup() {
    controller = new DMXController(13, 9);

    Serial.begin(115200);

    controller->add_channels(3);
}

void loop() {
    for (unsigned int i = 0; i < 3; ++i)
    {
        controller->write_channel(i, 0b01010101);
    }

    controller->write_frame();
    delay(3000);
}
