struct FadeChannel {
    uint8_t current = 0;

    uint8_t goal = 0;
    int32_t remaining_us = 0;

    using CallbackType = void(*)(FadeChannel&);
    CallbackType callback = nullptr;

    inline bool update(uint32_t dt) {
        Serial.print("current: ");
        Serial.print(current);
        Serial.print(" goal: ");
        Serial.print(goal);
        Serial.print(" remaining: ");
        Serial.print(remaining_us);
        Serial.print(" dt: ");
        Serial.println(dt);

        if (remaining_us <= 0 || current == goal || dt > static_cast<uint32_t>(remaining_us)) {
            current = goal;
            remaining_us = 0;

            return true;
        }

        const float diff = static_cast<float>(goal) - static_cast<float>(current);
        const float slope = diff / static_cast<float>(remaining_us);
        const uint16_t output = static_cast<uint16_t>(current) + dt * slope;

        remaining_us -= dt;

        if (output < 0) {
            current = 0;
        } else if (output > 255) {
            current = 255;
        } else {
            current = static_cast<uint8_t>(output);
        }
        return false;
    }
};

template <typename T>
T* move(T* in, size_t count) {
    return static_cast<T*>(realloc(in, sizeof(T) * count));
}

class DMXController {
public:
    DMXController(uint8_t pos_pin, uint8_t neg_pin) : pos_pin_(pos_pin), neg_pin_(neg_pin), time_us_(micros()) {
        pinMode(pos_pin, OUTPUT);
        pinMode(neg_pin, OUTPUT);

        // Zero channel is reserved!
        add_channel();
    }

    void add_channel() { add_channels(1); }
    void add_channels(uint16_t count) {
        num_channels_ += count;
        Serial.print("Adding channels: ");
        Serial.println(num_channels_);
        data_ = move(data_, num_channels_);
        Serial.println("Added channel!");
    }

    void channel_fade_to(uint16_t index, uint8_t value, uint32_t fade_us, FadeChannel::CallbackType callback=nullptr) {
        if (index < num_channels_) {
            FadeChannel& fade = data_[index];
            fade.goal = value;
            fade.remaining_us = fade_us;
            fade.callback = callback;
        }
    }
    void channel_write(uint16_t index, uint8_t value) {
        channel_fade_to(index, value, 0);
    }
    uint8_t channel_read(uint16_t index) const {
        if (index < num_channels_) {
            return data_[index].current;
        }
        return 0;
    }
    const FadeChannel* const channel_fade(uint16_t index) const {
        if (index < num_channels_) {
            return &data_[index];
        }
        return 0;
    }

    void write_frame()
    {
        const uint32_t dt_us = dt();

        // If we're exceeding the BREAK to BREAK time, delay here.
        if (dt_us < BREAK_TO_BREAK_US) {
            delayMicroseconds(BREAK_TO_BREAK_US - dt_us);
        }

        // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
        write_bit(0, SPACE_FOR_BREAK_US);

        // Mark After Break
        write_bit(1, MARK_AFTER_BREAK_US);

        // Start Code
        write_byte(0x00);

        // Now the rest of the channels
        for (uint16_t i = 1; i < num_channels_; ++i)
        {
            FadeChannel& channel = data_[i];
            if (channel.update(dt_us) && channel.callback) {
                channel.callback(channel);
                Serial.println("Done!");
            }
            write_byte(channel.current);
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

    uint32_t dt() {
        const uint32_t now_us = micros();
        uint32_t last_us = time_us_;
        time_us_ = now_us;

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

    uint16_t num_channels_ = 0;
    FadeChannel* data_ = nullptr;
};

