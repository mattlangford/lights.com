#include "lib/config/config.pb.h"

int main() {
    config::Node constant_node;
    constant_node.set_name("Constant");
    auto* cn = constant_node.mutable_constant_node();
    cn->add_values(1.0f);
    cn->add_values(2.0f);
    cn->add_values(3.0f);
}