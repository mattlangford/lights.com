#include <gtest/gtest.h>
#include "config.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include <cstring>

// Buffer size for encoding
constexpr size_t BUFFER_SIZE = 512;

// Encode a Runner message
bool encode_runner(uint8_t *buffer, size_t buffer_size, size_t *message_length) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

    // Create a Runner message
    config_Runner runner = config_Runner_init_default;

    // ---- Add ConstantNode ----
    runner.nodes_count = 3; // 3 nodes: Constant, Adder, Subtractor
    strcpy(runner.nodes[0].name, "ConstantNode");
    runner.nodes[0].which_config = config_Node_constant_node_tag;
    runner.nodes[0].config.constant_node.values_count = 2;
    runner.nodes[0].config.constant_node.values[0] = 1.0f;
    runner.nodes[0].config.constant_node.values[1] = 2.1f;

    // ---- Add AdderNode ----
    strcpy(runner.nodes[1].name, "AdderNode");
    runner.nodes[1].which_config = config_Node_adder_node_tag;
    runner.nodes[1].config.adder_node.saturating = false;
    runner.nodes[1].config.adder_node.inputs = 2;

    // ---- Add SubtractorNode ----
    strcpy(runner.nodes[2].name, "SubtractorNode");
    runner.nodes[2].which_config = config_Node_subtractor_node_tag;
    runner.nodes[2].config.subtractor_node.saturating = false;
    runner.nodes[2].config.subtractor_node.inputs = 2;

    // Encode
    if (!pb_encode(&stream, config_Runner_fields, &runner)) {
        return false;
    }

    *message_length = stream.bytes_written;
    return true;
}

// Decode a Runner message
bool decode_runner(const uint8_t *buffer, size_t message_length, Runner *runner) {
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
    return pb_decode(&stream, Runner_fields, runner);
}

// GTest for encoding and decoding Runner with ConstantNode, AdderNode, and SubtractorNode
TEST(NanopbTest, EncodeDecodeRunner) {
    uint8_t buffer[BUFFER_SIZE] = {0};
    size_t message_length = 0;

    // Encode
    ASSERT_TRUE(encode_runner(buffer, BUFFER_SIZE, &message_length));
    ASSERT_GT(message_length, 0);

    // Decode
    config_Runner decoded_runner = Runner_init_default;
    ASSERT_TRUE(decode_runner(buffer, message_length, &decoded_runner));

    // ---- Verify ConstantNode ----
    ASSERT_EQ(decoded_runner.nodes_count, 3);
    EXPECT_STREQ(decoded_runner.nodes[0].name, "ConstantNode");
    ASSERT_EQ(decoded_runner.nodes[0].which_config, config_Node_constant_node_tag);
    ASSERT_EQ(decoded_runner.nodes[0].config.constant_node.values_count, 2);
    EXPECT_FLOAT_EQ(decoded_runner.nodes[0].config.constant_node.values[0], 1.0f);
    EXPECT_FLOAT_EQ(decoded_runner.nodes[0].config.constant_node.values[1], 2.1f);

    // ---- Verify AdderNode ----
    EXPECT_STREQ(decoded_runner.nodes[1].name, "AdderNode");
    ASSERT_EQ(decoded_runner.nodes[1].which_config, config_Node_adder_node_tag);
    EXPECT_FALSE(decoded_runner.nodes[1].config.adder_node.saturating);
    EXPECT_EQ(decoded_runner.nodes[1].config.adder_node.inputs, 2);

    // ---- Verify SubtractorNode ----
    EXPECT_STREQ(decoded_runner.nodes[2].name, "SubtractorNode");
    ASSERT_EQ(decoded_runner.nodes[2].which_config, config_Node_subtractor_node_tag);
    EXPECT_FALSE(decoded_runner.nodes[2].config.subtractor_node.saturating);
    EXPECT_EQ(decoded_runner.nodes[2].config.subtractor_node.inputs, 2);
}
