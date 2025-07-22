#include <gtest/gtest.h>

// Simple test that doesn't require Vulkan runtime
TEST(SimpleTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(SimpleTest, StringComparison) {
    std::string hello = "Hello";
    EXPECT_EQ(hello, "Hello");
}

TEST(SimpleTest, BooleanTest) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}