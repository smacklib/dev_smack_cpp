#include <gtest/gtest.h> // googletest header file

#include <string>
#include <limits.h>

#include "test_common.hpp"
#include "src/smack_cli.hpp"

using std::string;

namespace {
    int f1(int p1) {
        return smack::test::common::f(__func__, p1);
    }

    int f2(int p1, const char* p2) {
        return smack::test::common::f(__func__, p1, p2);
    }

    int f3(int p1, double p2, const char* p3) {
        return smack::test::common::f(__func__, p1, p2, p3);
    }

    int f4(uint8_t p1) {
        return smack::test::common::f(__func__, p1);
    }
}

TEST(SmackCliTest, CommandHelpFull) {
    using smack::cli::Commands;

    auto cmd1 = Commands::make<f1>(
        "eins",
        { "p1" });

    auto help = cmd1.to_string();

    EXPECT_EQ( "eins p1:int", help);
}

TEST(SmackCliTest, CommandHelpNone) {
    using smack::cli::Commands;

    auto cmd = Commands::make<f2>(
        "zwei");

    auto help = cmd.to_string();

    EXPECT_EQ("zwei int, string", help);
}

TEST(SmackCliTest, CommandHelpPartial) {
    using smack::cli::Commands;

    auto cmd = Commands::make<f3>(
        "drei",
        { "p1", "p2" });

    auto help = cmd.to_string();

    EXPECT_EQ("drei p1:int, p2:double, string", help);
}

TEST(SmackCliTest, CommandUint8) {
    using smack::cli::Commands;

    auto cmd = Commands::make<f4>(
        "vier",
        { "uint8" });

    auto help = cmd.to_string();

    EXPECT_EQ("vier uint8:unsigned char", help);
}

TEST(SmackCliTest, TransformChar) {
    char out;
    smack::cli::transform("-128", out);
    EXPECT_EQ(-128, out);
    smack::cli::transform("127", out);
    EXPECT_EQ(127, out);
}

TEST(SmackCliTest, TransformCharFail) {
    try {
        const char* in = "dreizehn";
        char out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (std::invalid_argument(in)) {
        std::string exp{ "Cannot convert 'dreizehn' to char." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformCharFail2) {
    try {
        const char* in = "13e";
        char out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (const std::invalid_argument& in) {
        std::string exp{ "Cannot convert '13e' to char." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformCharRangeOverflow) {
    try {
        const char* in = "128";
        char out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (const std::invalid_argument& in) {
        std::string exp{ "Value 128 must be in range [-128..127]." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformCharRangeUnderflow) {
    try {
        const char* in = "-129";
        char out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (const std::invalid_argument& in) {
        std::string exp{ "Value -129 must be in range [-128..127]." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformInt) {
    int out;
    smack::cli::transform("-2147483648", out);
    
    EXPECT_EQ(std::numeric_limits<int>::min(), out);
    smack::cli::transform("2147483647", out);
    EXPECT_EQ(2147483647, out);
    smack::cli::transform("0x10", out);
    EXPECT_EQ(0x10, out);
}

TEST(SmackCliTest, TransformIntFail) {
    try {
        const char* in = "dreizehn";
        int out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (std::invalid_argument(in)) {
        std::string exp{ "Cannot convert 'dreizehn' to int." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformIntFail2) {
    try {
        const char* in = "13e";
        int out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (const std::invalid_argument& in) {
        std::string exp{ "Cannot convert '13e' to int." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformIntRangeOverflow) {
    try {
        const char* in = "2147483648";
        int out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (const std::invalid_argument& in) {
        std::string exp{ "Value 2147483648 must be in range [-2147483648..2147483647]." };
        EXPECT_EQ(exp, in.what());
    }
}

TEST(SmackCliTest, TransformIntRangeUnderflow) {
    try {
        const char* in = "-2147483649";
        int out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (const std::invalid_argument& in) {
        std::string exp{ "Value -2147483649 must be in range [-2147483648..2147483647]." };
        EXPECT_EQ(exp, in.what());
    }
}
