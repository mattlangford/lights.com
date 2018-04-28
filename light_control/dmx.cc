#include "dmx.hh"
#include <iostream>

namespace dmx
{
std::vector<bool> dmx_helper::generate_message_from_channels(const channels_t& channels)
{
    // we will keep an ordered buffer of the channel data in this vector, which will be transformed into
    // bits, headers added, and then serialized out
    // TODO: Might need to do some stuff with the zero channel
    std::vector<uint8_t> ordered_channels(MAX_NUM_CHANNELS);
    size_t max_address = 0;
    for (const channel_t& channel : channels)
    {
        if (channel.address > max_address)
        {
            max_address = channel.address;
        }

        ordered_channels[channel.address] = channel.level;
    }
    ordered_channels.resize(max_address + 1);

    std::vector<bool> full_bit_vector;

    // copy header, easy since it's at the start
    static const std::vector<bool> header = generate_header();
    std::copy(header.begin(), header.end(), std::back_inserter(full_bit_vector));

    // copy each channel in, a little more difficult
    for (const uint8_t& channel : ordered_channels)
    {
        const std::vector<bool> channel_bits = generate_channel(channel);

        std::copy(channel_bits.begin(), channel_bits.end(), std::back_inserter(full_bit_vector));
    }

    return full_bit_vector;
}

//
// ############################################################################
//

serial::ByteVector_t dmx_helper::simulate_differential_pair(const std::vector<bool>& bits)
{
    constexpr uint8_t HI_BIT = 0x01;
    constexpr uint8_t LO_BIT = 0x02;

    serial::ByteVector_t differential_pair_bytes;
    differential_pair_bytes.reserve(bits.size());
    for (const bool bit : bits)
    {
        differential_pair_bytes.push_back(bit ? HI_BIT : LO_BIT);
    }
    differential_pair_bytes.push_back(LO_BIT);

    return differential_pair_bytes;
}

//
// ############################################################################
//

std::vector<bool> dmx_helper::generate_header()
{
    std::vector<bool> header(HEADER_LENGTH);

    for (size_t i = 0; i < BREAK_SEQ_LEN_BITS; ++i)
    {
        constexpr bool BREAK_LEVEL = false;
        header[i] = BREAK_LEVEL;
    }
    for (size_t i = 0; i < MAB_SEQ_LEN_BITS; ++i)
    {
        constexpr int MAB_LEVEL = true;
        header[i + BREAK_SEQ_LEN_BITS] = MAB_LEVEL;
    }

    return header;
}

//
// ############################################################################
//

std::vector<bool> dmx_helper::generate_channel(const uint8_t level)
{
    // make sure we're at 250kb/s
    constexpr size_t CHANNEL_BITS_250KBS = 11;
    static_assert(CHANNEL_BITS == CHANNEL_BITS_250KBS, "Not supporting non-250kbs baud rate for the time being");

    std::vector<bool> channel(CHANNEL_BITS_250KBS);

    // start and stop bits
    constexpr bool START_BIT = false;
    channel[0] = START_BIT; // start bit is low
    constexpr bool STOP_BITS = true;
    channel[CHANNEL_BITS_250KBS - 2] = STOP_BITS; // last two bits are high
    channel[CHANNEL_BITS_250KBS - 1] = STOP_BITS;

    // now fill in the level
    uint8_t mask = 1;
    for (size_t i = 0; i < 8; ++i)
    {
        channel[i + 1] = mask & level;
        mask <<= 1;
    }

    return channel;
}

//
// ############################################################################
//

std::vector<uint8_t> dmx_helper::packet_bits(const std::vector<bool>& bits)
{
    const size_t num_bytes = bits.size() / BYTE_SIZE;

    std::vector<uint8_t> bytes(num_bytes, 0);
    for (size_t byte = 0; byte < num_bytes; ++byte)
    {
        size_t shift = BYTE_SIZE - 1;
        for (size_t bit = 0; bit < BYTE_SIZE; ++bit)
        {
            bytes[byte] += bits[byte * BYTE_SIZE + bit] << shift--;
        }
    }

    // If the number of bits is not divisible by the size of a byte, we need to add an extra
    // byte to the end
    const size_t extra_bits = bits.size() % BYTE_SIZE;
    if (extra_bits > 0)
    {
        bytes.push_back(0);
        uint8_t& last_byte = bytes.back();
        size_t shift = BYTE_SIZE - 1;
        for (size_t bit = 0; bit < extra_bits; ++bit)
        {
            last_byte += bits[bits.size() - extra_bits + bit] << shift--;
        }
    }

    return bytes;
}
}
