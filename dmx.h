// NOTE: It'll spend ~3 cycles loading and setting the next byte in during the WRITE() call, so subtract that off of the wait time
#define CYCLES_TO_US(cycles) 1E6 * (static_cast<double>(cycles) / F_CPU)
#define WRITE(value) PORTB = value; _dealy_us(4.0 - CYCLES_TO_US(3));

const static uint8_t kHigh = B10000000;
const static uint8_t kLow = B01010000;

void write_byte(uint8_t b)
{
  // The time between slots is undefined (at least up to 1s), so we'll do some preprocessing to make
  // the time critical parts run as quick as they need to.
  uint8_t transmit[8];
  for (uint8_t i = 0; i < 8; ++i)
    transmit[i] = b & (1u << i) ? kHigh : kLow;

  // Writing out these bits requires really accurate timing. For that reason I'm not using a for loop
  // since it appears that adds too much overhead.
  noInterrupts();
  WRITE(kLow);
  WRITE(transmit[0]);
  WRITE(transmit[1]);
  WRITE(transmit[2]);
  WRITE(transmit[3]);
  WRITE(transmit[4]);
  WRITE(transmit[5]);
  WRITE(transmit[6]);
  WRITE(transmit[7]);
  WRITE(kHigh);
  WRITE(kHigh);
  interrupts();
}

void write_break()
{
  PORTB = kLow;
  delayMicroseconds(176);
}
void write_mark_after_break()
{
  PORTB = kHigh;
  delayMicroseconds(15);
}
void write_mark_time_between_slots()
{
  delayMicroseconds(10); // artitrary
}
void write_mark_time_between_frames()
{
  PORTB = kHigh;
  delay(100); // long delay
}

void write_frame(const uint8_t* const data, const uint16_t num_slots)
{
  // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
  // Break
  write_break();

  // Mark After Break
  write_mark_after_break();

  // Start Code
  write_byte(0x00);

  // Now the rest of the slots
  for (uint16_t i = 0; i < num_slots; ++i)
  {
    write_byte(data[i]);
    write_mark_time_between_slots();
  }

  write_mark_time_between_frames();
}

void adjust_brightness(uint8_t& r, uint8_t& g, uint8_t& b, const float phase, float mult)
{
  float _min = 0.75;
  float _max = 1.0;
  float brightness = _min + (_max - _min) * (sin(phase) * mult + 1.0);
  r *= brightness;
  g *= brightness;
  b *= brightness;
}

const static unsigned int kSlotsPerLight = 7;
const static unsigned int kNumLights = 4;
const static unsigned int kPad = 10; // It seems like some lights need a few extra slots at the end, not sure why
const static unsigned int kTotalSlots = kSlotsPerLight * kNumLights + kPad;
