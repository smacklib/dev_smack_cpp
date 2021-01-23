#include <gtest/gtest.h> // googletest header file

#include <string>

#include "src/util_time_probe.hpp"

using std::string;

const char *actualValTrue  = "hello gtest";
const char *actualValFalse = "hello world";
const char *expectVal      = "hello gtest";

TEST(SmackUtilTest, CStrEqual) {
    EXPECT_STREQ(expectVal, actualValTrue);
}

TEST(SmackUtilTest, CStrNotEqual) {
    EXPECT_STREQ(expectVal, actualValFalse);
}
