#include <gtest/gtest.h>
#include "test.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include <vector>

// Buffer size for encoding
constexpr size_t BUFFER_SIZE = 128;

// Encode a SensorData message
bool encode_sensor_data(uint8_t *buffer, size_t buffer_size, size_t *message_length) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

    SensorData message = SensorData_init_default;
    message.id = 42;
    message.temperature = 23.5;
    message.humidity = 60.2;

    if (!pb_encode(&stream, SensorData_fields, &message)) {
        return false;
    }

    *message_length = stream.bytes_written;
    return true;
}

// Decode a SensorData message
bool decode_sensor_data(const uint8_t *buffer, size_t message_length, SensorData *message) {
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
    return pb_decode(&stream, SensorData_fields, message);
}

// GTest for encoding and decoding SensorData
TEST(NanopbTest, EncodeDecodeSensorData) {
    uint8_t buffer[BUFFER_SIZE] = {0};
    size_t message_length = 0;

    // Encode
    ASSERT_TRUE(encode_sensor_data(buffer, BUFFER_SIZE, &message_length));
    ASSERT_GT(message_length, 0);

    // Decode
    SensorData decoded_message = SensorData_init_default;
    ASSERT_TRUE(decode_sensor_data(buffer, message_length, &decoded_message));

    // Verify values
    EXPECT_EQ(decoded_message.id, 42);
    EXPECT_FLOAT_EQ(decoded_message.temperature, 23.5);
    EXPECT_FLOAT_EQ(decoded_message.humidity, 60.2);
}

// Encode a SystemMessage with SensorData
bool encode_system_message(uint8_t *buffer, size_t buffer_size, size_t *message_length) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

    SystemMessage message = SystemMessage_init_default;

    message.which_payload = SystemMessage_sensor_data_tag;
    message.payload.sensor_data.id = 99;
    message.payload.sensor_data.temperature = 30.1;
    message.payload.sensor_data.humidity = 50.5;

    if (!pb_encode(&stream, SystemMessage_fields, &message)) {
        return false;
    }

    *message_length = stream.bytes_written;
    return true;
}

// Decode a SystemMessage with SensorData
bool decode_system_message(const uint8_t *buffer, size_t message_length, SystemMessage *message) {
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
    return pb_decode(&stream, SystemMessage_fields, message);
}

// GTest for encoding and decoding SystemMessage
TEST(NanopbTest, EncodeDecodeSystemMessage) {
    uint8_t buffer[BUFFER_SIZE] = {0};
    size_t message_length = 0;

    // Encode
    ASSERT_TRUE(encode_system_message(buffer, BUFFER_SIZE, &message_length));
    ASSERT_GT(message_length, 0);

    // Decode
    SystemMessage decoded_message = SystemMessage_init_default;
    ASSERT_TRUE(decode_system_message(buffer, message_length, &decoded_message));

    // Verify values
    ASSERT_EQ(decoded_message.which_payload, SystemMessage_sensor_data_tag);
    EXPECT_EQ(decoded_message.payload.sensor_data.id, 99);
    EXPECT_FLOAT_EQ(decoded_message.payload.sensor_data.temperature, 30.1);
    EXPECT_FLOAT_EQ(decoded_message.payload.sensor_data.humidity, 50.5);
}