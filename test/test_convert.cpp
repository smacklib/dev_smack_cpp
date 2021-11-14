/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2019-2021 Michael Binz
 */

#include <gtest/gtest.h>

#include <limits.h>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#include "test_common.hpp"
#include "smack_convert.hpp"

using std::string;
using std::to_string;
using namespace smack::convert;

TEST(SmackConvert, TypenameTest) {

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

    EXPECT_EQ("unknown", string{ get_typename<void>() });

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
        transform( to_string(min).c_str(), out );
        EXPECT_EQ(min, out);
        transform( to_string(max).c_str(), out);
        EXPECT_EQ(max, out);
        // One inside range.
        transform( to_string(min+1).c_str(), out );
        EXPECT_EQ(min+1, out);
        transform( to_string(max-1).c_str(), out);
        EXPECT_EQ(max-1, out);
    }
    // Underflow.
    if ( min < 0 )
    {
        string belowMin = to_string(min);
        // Zero append is equivalent to multiply by ten.
        belowMin.append( "0" );

        try {
            transform( belowMin.c_str(), out );
            FAIL();
        }
        catch (const conversion_failure& e) {
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
            transform( overMax.c_str(), out );
            FAIL();
        }
        catch (const conversion_failure& e) {
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
            transform(in, out);
            FAIL();
        }
        catch (const conversion_failure& e) {
            string expected =
                "Cannot convert 'dreizehn' to " +
                string{ get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, e.what());
        }
    }
    // Number prefix.
    {
        try {
            const char* in = "13x";
            transform(in, out);
            FAIL();
        }
        catch (const conversion_failure& e) {
            string expected =
                "Cannot convert '13x' to " +
                string{ get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, e.what());
        }
    }
    // Hex notation.
    {
        const char* in = "0x13";
        transform(in, out);
        EXPECT_EQ(0x13, out);
    }
    // Bin notation.
    {
        const char* in = "0b00010011";
        transform(in, out);
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

        transform( to_string(max).c_str(), out);
        EXPECT_EQ(max, out);
        // One inside range.
        transform( to_string(min+1).c_str(), out );
        EXPECT_EQ(min+1, out);
        transform( to_string(max-1).c_str(), out);
        EXPECT_EQ(max-1, out);
    }
    // Bogus.
    {
        try {
            const char* in = "dreizehn";
            transform(in, out);
            FAIL();
        }
        catch (const conversion_failure& e) {
            string expected =
                "Cannot convert 'dreizehn' to " +
                string{ get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, e.what());
        }
    }
    // Number prefix.
    {
        try {
            const char* in = "13x";
            transform(in, out);
            FAIL();
        }
        catch (const conversion_failure& e) {
            string expected =
                "Cannot convert '13x' to " +
                string{ get_typename<T>() } +
                ".";
            EXPECT_EQ(expected, e.what());
        }
    }
}

TEST(SmackConvert, TransformBool) {
    bool out;

    transform("true", out);
    EXPECT_TRUE( out );
    transform("false", out);
    EXPECT_FALSE( out );

    try {
        const char* in = "13x";
        transform(in, out);
        FAIL();
    }
    catch (const conversion_failure& e) {
        string expected =
            "Cannot convert '13x' to " +
            string{ get_typename<bool>() } +
            ".";
        EXPECT_EQ(expected, e.what());
    }
}

TEST(SmackConvert, TransformChar) {
    testConversion<char>();
}

TEST(SmackConvert, TransformShort) {
    testConversion<short>();
}

TEST(SmackConvert, TransformInt) {
    testConversion<int>();
}

TEST(SmackConvert, TransformLong) {
    testConversion<long>();
}

TEST(SmackConvert, TransformLongLong) {
    testConversion<long long>();
}

TEST(SmackConvert, TransformUnsignedChar) {
    testConversion<unsigned char>();
}

TEST(SmackConvert, TransformUnsignedShort) {
    testConversion<unsigned short>();
}

TEST(SmackConvert, TransformUnsignedInt) {
    testConversion<unsigned int>();
}

TEST(SmackConvert, TransformUnsignedLong) {
    testConversion<unsigned long>();
}

TEST(SmackConvert, TransformUnsignedLongLong) {
    testConversion<unsigned long long>();
}

TEST(SmackConvert, TransformFloat) {
    testConversion<float>();
}
   
TEST(SmackConvert, TransformDouble) {
    testConversion<double>();
}

TEST(SmackConvert, TransformInt8) {
    testConversion<int8_t>();
}

TEST(SmackConvert, TransformUint8) {
    testConversion<uint8_t>();
}

TEST(SmackConvert, TransformInt16) {
    testConversion<int16_t>();
}

TEST(SmackConvert, TransformUint16) {
    testConversion<uint16_t>();
}

TEST(SmackConvert, TransformInt32) {
    testConversion<int32_t>();
}

TEST(SmackConvert, TransformUint32) {
    testConversion<uint32_t>();
}

TEST(SmackConvert, TransformInt64) {
    testConversion<int64_t>();
}

TEST(SmackConvert, TransformUint64) {
    testConversion<uint64_t>();
}

template<>
constexpr const char* smack::convert::get_typename( std::pair<float,float> type ) {
    return "floatPair"; 
}

TEST(SmackConvert, PairTypename) {

    std::pair<float, float> pair_;

    EXPECT_EQ("floatPair", string{ get_typename(pair_) });
}

template<> void smack::convert::transform(const char* in, std::pair<float, float>& out) {
    string input{ in };
    string delimiter{ ":" };

    auto pos = input.find( delimiter );

    if ( pos == string::npos )
        throw std::invalid_argument( in );

    auto first = 
        input.substr( 0, pos );
    auto second = 
        input.substr( pos + delimiter.length() );

    transform(
        first.c_str(),
        out.first );
    transform(
        second.c_str(),
        out.second );
}

TEST(SmackConvert, PairTransform) {
    std::pair<float, float> pair;

    transform( "3.0:4.0", pair );

    EXPECT_EQ( 3.0f, pair.first );
    EXPECT_EQ( 4.0f, pair.second );
}
