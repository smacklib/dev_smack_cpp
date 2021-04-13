#include <gtest/gtest.h> // googletest header file

#include <string>
#include <type_traits>
#include <limits.h>

#include "test_common.hpp"
#include "src/smack_cli.hpp"

using std::string;
using std::to_string;

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

TEST(SmackCliTest, TypenameTest) {
    using smack::cli::get_typename;

    EXPECT_EQ("bool", string{ get_typename<bool>() });

    EXPECT_EQ("ubyte", string{ get_typename<uint8_t>() });
    EXPECT_EQ("byte", string{ get_typename<int8_t>() });
    EXPECT_EQ("ubyte", string{ get_typename<unsigned char>() });
    EXPECT_EQ("byte", string{ get_typename<char>() });

    EXPECT_EQ("ushort", string{ get_typename<uint16_t>() });
    EXPECT_EQ("short", string{ get_typename<int16_t>() });
    EXPECT_EQ("ushort", string{ get_typename<unsigned short>() });
    EXPECT_EQ("short", string{ get_typename<short>() });

    EXPECT_EQ("uint", string{ get_typename<uint32_t>() });
    EXPECT_EQ("int", string{ get_typename<int32_t>() });
    EXPECT_EQ("uint", string{ get_typename<unsigned>() });
    EXPECT_EQ("int", string{ get_typename<int>() });

    EXPECT_EQ("ulong", string{ get_typename<uint64_t>() });
    EXPECT_EQ("long", string{ get_typename<int64_t>() });
    EXPECT_EQ("ulong", string{ get_typename<unsigned long>() });
    EXPECT_EQ("long", string{ get_typename<long>() });

    EXPECT_EQ("float", string{ get_typename<float>() });
    EXPECT_EQ("double", string{ get_typename<double>() });
    EXPECT_EQ("ldouble", string{ get_typename<long double>() });

    EXPECT_EQ("string", string{ get_typename<std::string>() });
    EXPECT_EQ("string", string{ get_typename<char*>() });
    EXPECT_EQ("string", string{ get_typename<const char*>() });
    EXPECT_EQ("string", string{ get_typename<std::string&>() });
    EXPECT_EQ("string", string{ get_typename<const std::string&>() });

    EXPECT_EQ("whatever", string{ get_typename<void>() });

    EXPECT_EQ("string", string{ get_typename<char*>() });
    EXPECT_EQ("string", string{ get_typename<const char*>() });

    EXPECT_EQ("int", string{ get_typename<int&>() });
    EXPECT_EQ("int", string{ get_typename<const int&>() });
}

template <typename T>
void testConversion()
{
    T min = std::numeric_limits<T>::min();
    T max = std::numeric_limits<T>::max();

    T out;

    // Test conversion in the range of T.
    {
        // Border cases.
        smack::cli::transform( to_string(min).c_str(), out );
        EXPECT_EQ(min, out);
        smack::cli::transform( to_string(max).c_str(), out);
        EXPECT_EQ(max, out);
        // One inside range.
        smack::cli::transform( to_string(min+1).c_str(), out );
        EXPECT_EQ(min+1, out);
        smack::cli::transform( to_string(max-1).c_str(), out);
        EXPECT_EQ(max-1, out);
    }
    // Underflow.
    {
        long long belowMin = min-1;
        try {
            smack::cli::transform( to_string(belowMin).c_str(), out );
            FAIL();
        }
        catch (const std::invalid_argument& e) {
            string expected = 
                "Value " +
                to_string( belowMin ) +
                " must be in range [" +
                to_string( min ) +
                ".." +
                to_string( max ) +
                "].";
            EXPECT_EQ(expected, e.what());
        }
    }
    // Overflow.
    {
        long long overMax = max+1;
        try {
            smack::cli::transform( to_string(overMax).c_str(), out );
            FAIL();
        }
        catch (const std::invalid_argument& e) {
            string expected = 
                "Value " +
                to_string( overMax ) +
                " must be in range [" +
                to_string( min ) +
                ".." +
                to_string( max ) +
                "].";
            EXPECT_EQ(expected, e.what());
        }
    }
    // Bogus.
    {
        try {
            const char* in = "dreizehn";
            smack::cli::transform(in, out);
            FAIL();
        }
        catch (std::invalid_argument(in)) {
            std::string exp{ "Cannot convert 'dreizehn' to char." };
            EXPECT_EQ(exp, in.what());
        }
    }
}

TEST(SmackCliTest, TransformChar) {
    testConversion<char>();
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
