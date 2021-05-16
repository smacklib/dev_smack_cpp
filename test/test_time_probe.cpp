#include <gtest/gtest.h> // googletest header file

#include <string>

#include "smack_util_time_probe.hpp"

using std::string;

namespace {
    const char* actualValTrue = "hello gtest";
    const char* actualValFalse = "hello world";
    const char* expectVal = "hello gtest";
};

TEST(StrCompare, CStrEqual) {
    EXPECT_STREQ(expectVal, actualValTrue);
}

TEST(StrCompare, CStrNotEqual) {
    EXPECT_STRNE(expectVal, actualValFalse);
}
