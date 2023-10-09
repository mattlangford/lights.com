#include "dmx.h"


uint8_t lights_aurora[kTotalSlots] = {
  255, 255, 15, 70, 0, 0, 0,
  255, 50, 90, 200, 0, 0, 0,
  255, 255, 30, 50, 0, 0, 0,
  255, 30, 50, 200, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t lights_sunset[kTotalSlots] = {
  255, 150, 100, 30, 0, 0, 0,
  255, 150, 50, 75, 0, 0, 0,
  255, 150, 50, 75, 0, 0, 0,
  255, 150, 100, 30, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t lights_halloween[kTotalSlots] = {
  0,  255, 50, 10, 0, 0, 0,
  0, 128, 0, 128, 0, 0, 0,
  0, 255, 50, 10, 0, 0, 0,
  0, 128, 0, 128, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t* lights;

static float phase = 0;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  Serial.begin(115200);

  lights = lights_aurora;
}

void loop() {
  phase += 0.01;
  while (phase >= 2 * PI) phase -= 2 * PI;

  for (int light = 0; light < kNumLights; ++light)
  {

    float _min = 0.75;
    float _max = 1.0;
    float brightness = _min + (_max - _min) * sin(phase + light);

    auto base = kSlotsPerLight * light;
    lights[base] = 255 * brightness;
  }

  write_frame(lights, kTotalSlots);
}
