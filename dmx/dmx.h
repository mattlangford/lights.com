#define DATA_POS_PIN 13
#define DATA_NEG_PIN 9

// This method copies the delayMicroseconds method, but includes the digitalWriteFast calls.
// With an oscope, this shows about 30ns of error (or 12 instructions) which I've adjusted for here.
static inline void write_bit(bool value, uint32_t delay_us) {
    uint32_t begin = ARM_DWT_CYCCNT;
    digitalWriteFast(DATA_POS_PIN, value);
    digitalWriteFast(DATA_NEG_PIN, !value);
    uint32_t cycles = F_CPU_ACTUAL / 1000000 * delay_us;
    while (ARM_DWT_CYCCNT - begin < cycles - 12); // wait
}

// Delays defined in microseconds
// [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
#define BIT_DELAY 4
#define SPACE_FOR_BREAK_DELAY 92
#define MARK_AFTER_BREAK_DELAY 12
#define MARK_BEFORE_BREAK_DELAY 100 // arbitrary

void write_byte(uint8_t b)
{
  bool bits[8];
  for (uint8_t i = 0; i < 8; ++i) {
      bits[i] = (b & (1u << i)) != 0;
  }

  noInterrupts();
  write_bit(0, BIT_DELAY);

  write_bit(bits[0], BIT_DELAY);
  write_bit(bits[1], BIT_DELAY);
  write_bit(bits[2], BIT_DELAY);
  write_bit(bits[3], BIT_DELAY);
  write_bit(bits[4], BIT_DELAY);
  write_bit(bits[5], BIT_DELAY);
  write_bit(bits[6], BIT_DELAY);
  write_bit(bits[7], BIT_DELAY);

  write_bit(1, 2 * BIT_DELAY);
  interrupts();
}

void write_frame(const uint8_t* const data, const uint16_t num_slots)
{
  // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
  write_bit(0, SPACE_FOR_BREAK_DELAY);

  // Mark After Break
  write_bit(1, MARK_AFTER_BREAK_DELAY);

  // Start Code
  write_byte(0x00);

  // Now the rest of the slots
  for (uint16_t i = 0; i < num_slots; ++i)
  {
    write_byte(data[i]);
  }

  write_bit(1, MARK_BEFORE_BREAK_DELAY);
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
