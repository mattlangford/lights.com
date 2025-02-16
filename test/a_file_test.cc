#include <gtest/gtest.h>
#include "a_file.hh"

TEST(MyModuleTest, TestFunction) {
    Tester t;
    t.test() = 23;
    ASSERT_EQ(t.test(), 23);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
