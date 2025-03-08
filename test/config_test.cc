#include <gtest/gtest.h>
#include "config.pb.h"

std::string serialize() {
    config::Runner runner;
    
    // Create a ConstantNode with three values
    config::Node constant_node;
    constant_node.set_name("Constant");
    auto* cn = constant_node.mutable_constant_node();
    cn->add_values(1.0f);
    cn->add_values(2.0f);
    cn->add_values(3.0f);
    
    // Create an AdderNode
    config::Node adder_node;
    adder_node.set_name("Adder");
    auto* an = adder_node.mutable_adder_node();
    an->set_saturating(true);
    an->set_inputs(2);
    
    // Create a SubtractorNode
    config::Node subtractor_node;
    subtractor_node.set_name("Subtractor");
    auto* sn = subtractor_node.mutable_subtractor_node();
    sn->set_saturating(false);
    sn->set_inputs(2);
    
    // Add nodes to the runner
    *runner.add_nodes() = constant_node;
    *runner.add_nodes() = adder_node;
    *runner.add_nodes() = subtractor_node;
    
    // Create connections
    config::Connection conn1;
    conn1.set_from_node(0); // ConstantNode index
    conn1.set_from_output(0);
    conn1.set_to_node(1);   // AdderNode index
    conn1.set_to_input(0);
    *runner.add_connections() = conn1;
    
    config::Connection conn2;
    conn2.set_from_node(1); // AdderNode index
    conn2.set_from_output(0);
    conn2.set_to_node(2);   // SubtractorNode index
    conn2.set_to_input(0);
    *runner.add_connections() = conn2;

    std::string output;
    if (!runner.SerializeToString(&output)) {
        std::cerr << "Failed to serialize message.\n";
    } else {
        std::cout << "Message serialized to string (" << output.size() << " bytes).\n";
    }
    return output;
}

TEST(RunnerTest, BasicSetup) {
    std::string s = serialize();

    config::Runner msg;
    EXPECT_TRUE(!msg.ParseFromString(s));
    
    // Validate deserialized data
    ASSERT_EQ(msg.nodes_size(), 3);
    ASSERT_EQ(msg.connections_size(), 2);
    EXPECT_EQ(msg.nodes(0).name(), "Constant");
    EXPECT_EQ(msg.nodes(1).name(), "Adder");
    EXPECT_EQ(msg.nodes(2).name(), "Subtractor");
}
