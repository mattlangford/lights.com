#pragma once

#include "util.hh"

// Many effects have some min/max bounds, this struct allows easy reuse for RGB effects.
struct ValueConfig {
    uint8_t min = 0;
    uint8_t max = 255;

    static ValueConfig fixed(uint8_t v) { return {.min=v, .max=v}; }
};

class Effect {
public:
    virtual ~Effect() = default;

    virtual uint8_t process(uint8_t value, uint32_t now_ms) = 0;
    virtual void trigger(uint32_t now_ms) {};
    virtual void clear(uint32_t now_ms) {};

    void set_values(const ValueConfig& values) { values_ = values; }

protected:
    uint32_t now() const { return millis(); }
    uint8_t clip(float in) const { return in < 0 ? 0 : in > 255 ? 255 : static_cast<uint8_t>(in); }

    inline uint8_t min_value() const { return values_.min; }
    inline uint8_t max_value() const { return values_.max; }

private:
    ValueConfig values_;
};

class Channel {
public:
    ~Channel() {
        for (size_t i = 0; i < count_; ++i) { delete effects_[i]; }
        if (effects_) { delete effects_; }
    }

    uint8_t get_value(uint32_t now_ms) {
        for (uint8_t i = 0; i < count_; ++i) {
            if (Effect* effect = effects_[i]) {
                value_ = effect->process(value_, now_ms);
            }
        }
        return value_;
    }

    void set_value(uint8_t value) { value_ = value; }


    Effect* add_effect(Effect* effect) {
        uint8_t last = count_++;
        effects_ = move(effects_, count_);
        effects_[last] = effect;
        return effect;
    }

    Effect* effect(uint8_t layer) const {
        return layer < count_ ? effects_[layer] : nullptr;
    }

private:
    uint8_t value_ = 0;

    // Owned here
    uint8_t count_ = 0;
    Effect** effects_ = nullptr;
};

class DMXController {
public:
    static constexpr uint16_t MAX_CHANNELS = 512;

    DMXController(uint8_t pos_pin, uint8_t neg_pin) : pos_pin_(pos_pin), neg_pin_(neg_pin), time_us_(micros()) {
        pinMode(pos_pin, OUTPUT);
        pinMode(neg_pin, OUTPUT);
    }

    void set_max_channel(uint16_t index) {
        if (index <= MAX_CHANNELS) {
            channel_count_ = index > channel_count_ ? index : channel_count_;
        }
    }
    Channel& channel(uint16_t index) {
        set_max_channel(index);
        return channels_[index];
    }

    void write_frame()
    {
        uint32_t dt_us = dt();

        // If we're exceeding the BREAK to BREAK time, don't send an update.
        if (dt_us < BREAK_TO_BREAK_US) {
            return;
        }

        time_us_ = micros();
        uint32_t now_ms = time_us_ / 1000;

        // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
        write_bit(0, SPACE_FOR_BREAK_US);

        // Mark After Break
        write_bit(1, MARK_AFTER_BREAK_US);

        // Start Code
        write_byte(0x00);

        // Now the rest of the channels
        for (uint16_t i = 1; i <= channel_count_; ++i) {
            // Compute the value of this channel, the time between slots is arbitrary so do the processing here.
            uint8_t value = channels_[i].get_value(now_ms);
            Serial.print(i);
            Serial.print(": ");
            Serial.println(value);
            write_byte(value);
        }

        write_bit(1, MARK_BEFORE_BREAK_US);

    }

private:
    // Delays defined in microseconds
    // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
    static constexpr uint32_t BIT_US = 4;
    static constexpr uint32_t SPACE_FOR_BREAK_US = 92;
    static constexpr uint32_t MARK_AFTER_BREAK_US = 12;
    static constexpr uint32_t MARK_BEFORE_BREAK_US = 100; // arbitrary
    static constexpr uint32_t BREAK_TO_BREAK_US = 1204;

    // This method copies the delayMicroseconds method, but includes the digitalWriteFast calls.
    // With an oscope, this shows about 30ns of error (or 12 instructions) which I've adjusted for here.
    inline void write_bit(bool value, uint32_t delay_us) const {
        uint32_t begin = ARM_DWT_CYCCNT;
        digitalWriteFast(pos_pin_, value);
        digitalWriteFast(neg_pin_, !value);
        uint32_t cycles = F_CPU_ACTUAL / 1000000 * delay_us;
        while (ARM_DWT_CYCCNT - begin < cycles - 12); // wait
    }

    void write_byte(uint8_t b) const
    {
        // The mark time between slots is arbitrary, so use it to pre-process.
        bool bits[8];
        for (uint8_t i = 0; i < 8; ++i) {
            bits[i] = (b & (1u << i)) != 0;
        }

        noInterrupts();
        write_bit(0, BIT_US);

        // Unrolling the loop so the timing is a bit more straightforward.
        write_bit(bits[0], BIT_US);
        write_bit(bits[1], BIT_US);
        write_bit(bits[2], BIT_US);
        write_bit(bits[3], BIT_US);
        write_bit(bits[4], BIT_US);
        write_bit(bits[5], BIT_US);
        write_bit(bits[6], BIT_US);
        write_bit(bits[7], BIT_US);

        write_bit(1, 2 * BIT_US);
        interrupts();
    }

    uint32_t dt() const {
        const uint32_t now_us = micros();
        uint32_t last_us = time_us_;

        if (now_us > last_us) {
            return now_us - last_us;
        } else {
            // Time wrapped! Assume dt is small and just calculate how much it wrapped
            return (static_cast<uint32_t>(-1) - last_us) + now_us;
        }
    }

private:
    uint8_t pos_pin_;
    uint8_t neg_pin_;

    uint32_t time_us_;

    uint16_t channel_count_ = 0;
    Channel channels_[MAX_CHANNELS + 1];
};

