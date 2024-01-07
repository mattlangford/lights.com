#pragma once

#include <vector>

class ChannelEffect {
public:
    virtual ~ChannelEffect() = default;

    uint8_t process(uint8_t value, uint32_t now_ms) {
        return clip((max_ - min_) * level(now_ms) + min_ + input_gain_ * value);
    }

    void set_values(uint8_t min, uint8_t max) { set_min(min); set_max(max); }
    void set_min(uint8_t min) { min_ = min; }
    void set_max(uint8_t max) { max_ = max; }
    void set_input_gain(float gain) { input_gain_ = gain; }

protected:
    uint8_t clip(float in) const { return in < 0 ? 0 : in > 255 ? 255 : static_cast<uint8_t>(in); }

    uint8_t min_value() const { return min_; }
    uint8_t max_value() const { return max_; }
    float input_gain() const { return input_gain_; }

    // Return the current level, between 0.0 and 1.0
    virtual float level(uint32_t now_ms) { return 0.0; }

private:
    float input_gain_ = 1.0;
    uint8_t min_ = 0;
    uint8_t max_ = 255;
};

///
/// @brief Each Channel is composed of a stack of ChannelEffects which are processed serially to
///        generate the value at each timestamp. The channel does NOT own its effects.
///
class Channel {
public:
    uint8_t get_value(uint32_t now_ms) {
        uint8_t value = value_;
        for (auto* effect : effects_) {
            value = effect->process(value, now_ms);
        }
        return value;
    }

    void set_value(uint8_t value) { value_ = value; }

    void add_effect(ChannelEffect* effect) {
        effects_.push_back(effect);
    }

    template <typename Effect=ChannelEffect>
    Effect* effect(uint8_t layer) const {
        return layer < effects_.size() ? dynamic_cast<ChannelEffect*>(effects_[layer]) : nullptr;
    }

private:
    uint8_t value_ = 0;
    std::vector<ChannelEffect*> effects_;
};

///
/// @brief Main controller class, call write_frame periodically to query the latest state and write it.
///        Computing channel values is done between writing slots, since the spec allows for flexible gaps.
///
class DMXController {
public:
    static constexpr uint16_t MAX_CHANNELS = 512;

    DMXController(uint8_t pos_pin, uint8_t neg_pin) : pos_pin_(pos_pin), neg_pin_(neg_pin), time_us_(micros()) {
        pinMode(pos_pin, OUTPUT);
        pinMode(neg_pin, OUTPUT);
    }

    uint16_t max_channel() const { return channel_count_; }
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
            write_byte(value);
        }

        // Padding, it seems some lights don't like being the last channel.
        for (int16_t i = channel_count_; i < 10; ++i)  {
            if (i > 512) break;
            write_byte(0);
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

