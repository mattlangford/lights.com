class DMXController {
public:
    DMXController(uint8_t pos_pin, uint8_t neg_pin) : pos_pin_(pos_pin), neg_pin_(neg_pin) {
        pinMode(pos_pin, OUTPUT);
        pinMode(neg_pin, OUTPUT);
    }

    void add_channels(uint16_t count=1) {
        num_channels_ += count;
        data_ = static_cast<uint8_t*>(realloc(data_, num_channels_));
    }

    void write_channel(uint16_t index, uint8_t value) {
        if (index < num_channels_) {
            data_[index] = value;
        }
    }
    uint8_t read_channel(uint16_t index) const {
        if (index < num_channels_) {
            return data_[index];
        }
        return 0;
    }

    void write_frame() const
    {
        // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
        write_bit(0, SPACE_FOR_BREAK_DELAY);

        // Mark After Break
        write_bit(1, MARK_AFTER_BREAK_DELAY);

        // Start Code
        write_byte(0x00);

        // Now the rest of the channels
        for (uint16_t i = 0; i < num_channels_; ++i)
        {
            write_byte(data_[i]);
        }

        write_bit(1, MARK_BEFORE_BREAK_DELAY);
    }

private:
    // Delays defined in microseconds
    // [1]: https://tsp.esta.org/tsp/documents/docs/ANSI-ESTA_E1-11_2008R2018.pdf
    static constexpr uint32_t BIT_DELAY = 4;
    static constexpr uint32_t SPACE_FOR_BREAK_DELAY = 92;
    static constexpr uint32_t MARK_AFTER_BREAK_DELAY = 12;
    static constexpr uint32_t MARK_BEFORE_BREAK_DELAY = 100;

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

private:
    uint8_t pos_pin_;
    uint8_t neg_pin_;

    uint16_t num_channels_ = 0;
    uint8_t* data_ = nullptr;
};

