#include <gtest/gtest.h> // googletest header file

#include <limits.h>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

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

    int fPair(std::pair<int,int> p1) {
        return smack::test::common::f(__func__, p1.first, p1.second);
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

    // Test the integer definitions with explit width.
    EXPECT_EQ("ubyte", string{ get_typename<uint8_t>() });
    EXPECT_EQ("byte", string{ get_typename<int8_t>() });
    EXPECT_EQ("ushort", string{ get_typename<uint16_t>() });
    EXPECT_EQ("short", string{ get_typename<int16_t>() });
    EXPECT_EQ("uint", string{ get_typename<uint32_t>() });
    EXPECT_EQ("int", string{ get_typename<int32_t>() });
    EXPECT_EQ("ulong", string{ get_typename<uint64_t>() });
    EXPECT_EQ("long", string{ get_typename<int64_t>() });

    // Test the plain primitive types.
    EXPECT_EQ("bool", string{ get_typename<bool>() });

    EXPECT_EQ("ubyte", string{ get_typename<unsigned char>() });
    EXPECT_EQ("byte", string{ get_typename<char>() });

    EXPECT_EQ("ushort", string{ get_typename<unsigned short>() });
    EXPECT_EQ("short", string{ get_typename<short>() });

    EXPECT_EQ("uint", string{ get_typename<unsigned>() });
    EXPECT_EQ("int", string{ get_typename<int>() });

#ifndef _WIN32
    // On Windows long is equivalent to int.
    EXPECT_EQ("ulong", string{ get_typename<unsigned long>() });
    EXPECT_EQ("long", string{ get_typename<long>() });
#endif

    EXPECT_EQ("float", string{ get_typename<float>() });
    EXPECT_EQ("double", string{ get_typename<double>() });
#ifndef _WIN32
    // On Windows long double is equivalent to double.
    EXPECT_EQ("ldouble", string{ get_typename<long double>() });
#endif

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

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
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
    if ( min < 0 )
    {
        string belowMin = to_string(min);
        // Zero append is equivalent to multiply by ten.
        belowMin.append( "0" );

        try {
            smack::cli::transform( belowMin.c_str(), out );
            FAIL();
        }
        catch (const std::invalid_argument& e) {
            string expected = 
                "Value " +
                belowMin +
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
        string overMax = to_string( max );
        // Zero append is equivalent to multiply by ten.
        overMax.append( "0" );

        try {
            smack::cli::transform( overMax.c_str(), out );
            FAIL();
        }
        catch (const std::invalid_argument& e) {
            string expected = 
                "Value " +
                overMax +
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
            string expected =
                "Cannot convert 'dreizehn' to " +
                string{ smack::cli::get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, in.what());
        }
    }
    // Number prefix.
    {
        try {
            const char* in = "13x";
            smack::cli::transform(in, out);
            FAIL();
        }
        catch (std::invalid_argument(in)) {
            string expected =
                "Cannot convert '13x' to " +
                string{ smack::cli::get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, in.what());
        }
    }
    // Hex notation.
    {
        const char* in = "0x13";
        smack::cli::transform(in, out);
        EXPECT_EQ(0x13, out);
    }
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
void testConversion()
{
    static_assert(
        std::is_floating_point<T>::value,
        "T must be a floating point type." );

    T out;

    // Test conversion in the range of T.
    {
        T min = 1.0;
        T max = std::numeric_limits<T>::max();

        smack::cli::transform( to_string(max).c_str(), out);
        EXPECT_EQ(max, out);
        // One inside range.
        smack::cli::transform( to_string(min+1).c_str(), out );
        EXPECT_EQ(min+1, out);
        smack::cli::transform( to_string(max-1).c_str(), out);
        EXPECT_EQ(max-1, out);
    }
    // Bogus.
    {
        try {
            const char* in = "dreizehn";
            smack::cli::transform(in, out);
            FAIL();
        }
        catch (std::invalid_argument(in)) {
            string expected =
                "Cannot convert 'dreizehn' to " +
                string{ smack::cli::get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, in.what());
        }
    }
    // Number prefix.
    {
        try {
            const char* in = "13x";
            smack::cli::transform(in, out);
            FAIL();
        }
        catch (std::invalid_argument(in)) {
            string expected =
                "Cannot convert '13x' to " +
                string{ smack::cli::get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, in.what());
        }
    }
}

TEST(SmackCliTest, TransformBool) {
    bool out;

    smack::cli::transform("true", out);
    EXPECT_TRUE( out );
    smack::cli::transform("false", out);
    EXPECT_FALSE( out );

    try {
        const char* in = "13x";
        smack::cli::transform(in, out);
        FAIL();
    }
    catch (std::invalid_argument(in)) {
        string expected =
            "Cannot convert '13x' to " +
            string{ smack::cli::get_typename<bool>() } +
            ".";
        EXPECT_EQ(expected, in.what());
    }
}

TEST(SmackCliTest, TransformChar) {
    testConversion<char>();
}

TEST(SmackCliTest, TransformShort) {
    testConversion<short>();
}

TEST(SmackCliTest, TransformInt) {
    testConversion<int>();
}

TEST(SmackCliTest, TransformLong) {
    testConversion<long>();
}

TEST(SmackCliTest, TransformLongLong) {
    testConversion<long long>();
}

TEST(SmackCliTest, TransformUnsignedChar) {
    testConversion<unsigned char>();
}

TEST(SmackCliTest, TransformUnsignedShort) {
    testConversion<unsigned short>();
}

TEST(SmackCliTest, TransformUnsignedInt) {
    testConversion<unsigned int>();
}

TEST(SmackCliTest, TransformUnsignedLong) {
    testConversion<unsigned long>();
}

TEST(SmackCliTest, TransformUnsignedLongLong) {
    testConversion<unsigned long long>();
}

TEST(SmackCliTest, TransformFloat) {
    testConversion<float>();
}
   
TEST(SmackCliTest, TransformDouble) {
    testConversion<double>();
}

TEST(SmackCliTest, TransformInt8) {
    testConversion<int8_t>();
}

TEST(SmackCliTest, TransformUint8) {
    testConversion<uint8_t>();
}

TEST(SmackCliTest, TransformInt16) {
    testConversion<int16_t>();
}

TEST(SmackCliTest, TransformUint16) {
    testConversion<uint16_t>();
}

TEST(SmackCliTest, TransformInt32) {
    testConversion<int32_t>();
}

TEST(SmackCliTest, TransformUint32) {
    testConversion<uint32_t>();
}

TEST(SmackCliTest, TransformInt64) {
    testConversion<int64_t>();
}

TEST(SmackCliTest, TransformUint64) {
    testConversion<uint64_t>();
}

template<>
constexpr const char* smack::cli::get_typename( std::pair<int,int> type ) { 
    return "pair"; 
}

TEST(SmackCliTest, PairTypename) {
    using smack::cli::get_typename;

    std::pair<int,int> pair_;

    EXPECT_EQ("pair", string{ get_typename(pair_) });
}

template<> void smack::cli::transform(const char* in, std::pair<int,int>& out) {
    string input{ in };
    string delimiter{ ":" };

    auto pos = input.find( delimiter );

    if ( pos == string::npos )
        throw std::invalid_argument( in );

    auto first = 
        input.substr( 0, pos );
    auto second = 
        input.substr( pos + delimiter.length() );

    smack::cli::transform(
        first.c_str(),
        out.first );
    smack::cli::transform(
        second.c_str(),
        out.second );
}

TEST(SmackCliTest, PairTransform) {
    std::pair<int,int> pair;

    smack::cli::transform( "3:4", pair );

    EXPECT_EQ( 3, pair.first );
    EXPECT_EQ( 4, pair.second );
}

TEST(SmackCliTest, CommandPairHelp) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    auto help = cmd.to_string();

    EXPECT_EQ("fPair p1:pair", help);

    std::vector<string> argv{"212:313"};

    // Redirect stdout.
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    cmd( argv );
    // Get stout content.
    std::string text = buffer.str();

    EXPECT_EQ("fPair( 212, 313 )\n", text);

    std::cout.rdbuf(old);
}

TEST(SmackCliTest, CommandPairExec) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    std::vector<string> argv{ "212:313" };

    // Redirect stdout.
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Execute the command.
    cmd(argv);

    // Get stout content.
    std::string text = buffer.str();

    EXPECT_EQ("fPair( 212, 313 )\n", text);

    std::cout.rdbuf(old);
}

TEST(SmackCliTest, CommandPairExecCli) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    std::vector<string> argv{
        cmd.get_name(),
        "212:313" };

    auto cli = smack::cli::makeCliApplication(
        cmd
    );

    // Redirect stdout.
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_SUCCESS, exitCode);

    // Get stdout content.
    std::string text = buffer.str();

    EXPECT_EQ("fPair( 212, 313 )\n", text);

    std::cout.rdbuf(old);
}
