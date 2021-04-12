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

template <typename T,
            std::enable_if_t<std::is_integral<T>::value, bool> = true
>
const char* typename_b()
{
    if ( std::numeric_limits<T>::digits == 1 )
        return "bool";

    switch ( sizeof( T ) ) {
        case 1: {
            return std::numeric_limits<T>::min() == 0 ?
                "ubyte" : "byte";
        }
        case 2: {
            return std::numeric_limits<T>::min() == 0 ?
                "ushort" : "short";
        }
        case 4: {
            return std::numeric_limits<T>::min() == 0 ?
                "uint" : "int";
        }
        case 8: {
            return std::numeric_limits<T>::min() == 0 ?
                "ulong" : "long";
        }
        default:
            return "badInt";
    }

    return "badint";
}

template <typename T,
            std::enable_if_t<std::is_floating_point<T>::value, bool> = true
>
const char* typename_b()
{
    switch (std::numeric_limits<T>::digits)
    {
    case std::numeric_limits<float>::digits:
        return "float";
    case std::numeric_limits<double>::digits:
        return "double";
    case std::numeric_limits<long double>::digits:
        return "ldouble";
    }

    return "badfloat";
}

template <typename T,
            std::enable_if_t<std::is_same<T,char*>::value, bool> = true
>
constexpr const char* typename_b()
{
    return "string";
}

template <typename T,
            std::enable_if_t<std::is_same<T,std::string>::value, bool> = true
>
const char* typename_b()
{
    return "string";
}

template <typename T,
            std::enable_if_t<std::is_pointer<T>::value, bool> = true
>
const char* typename_c()
{
    using i1 = typename std::remove_pointer<T>::type;
    using i2 = typename std::remove_const<i1>::type;
    using i3 = typename std::add_pointer<i2>::type;

    return typename_b<i3>();
}
template <typename T,
            std::enable_if_t<!std::is_pointer<T>::value, bool> = true
>
const char* typename_c()
{
    return typename_b<T>();
}

TEST(SmackCliTest, PrimitiveNames) {
    using smack::cli::Commands;

    EXPECT_EQ("bool", string{ typename_c<bool>() });

    EXPECT_EQ("ubyte", string{ typename_c<uint8_t>() });
    EXPECT_EQ("byte", string{ typename_c<int8_t>() });
    EXPECT_EQ("ubyte", string{ typename_c<unsigned char>() });
    EXPECT_EQ("byte", string{ typename_c<char>() });

    EXPECT_EQ("ushort", string{ typename_c<uint16_t>() });
    EXPECT_EQ("short", string{ typename_c<int16_t>() });
    EXPECT_EQ("ushort", string{ typename_c<unsigned short>() });
    EXPECT_EQ("short", string{ typename_c<short>() });

    EXPECT_EQ("uint", string{ typename_c<uint32_t>() });
    EXPECT_EQ("int", string{ typename_c<int32_t>() });
    EXPECT_EQ("uint", string{ typename_c<unsigned>() });
    EXPECT_EQ("int", string{ typename_c<int>() });

    EXPECT_EQ("ulong", string{ typename_c<uint64_t>() });
    EXPECT_EQ("long", string{ typename_c<int64_t>() });
    EXPECT_EQ("ulong", string{ typename_c<unsigned long>() });
    EXPECT_EQ("long", string{ typename_c<long>() });

    EXPECT_EQ("float", string{ typename_c<float>() });
    EXPECT_EQ("double", string{ typename_c<double>() });
    EXPECT_EQ("ldouble", string{ typename_c<long double>() });

    EXPECT_EQ("string", string{ typename_c<std::string>() });
    EXPECT_EQ("string", string{ typename_c<char*>() });
    EXPECT_EQ("string", string{ typename_c<const char*>() });

    // auto cmd = Commands::make<f4>(
    //     "vier",
    //     { "uint8" });

    // auto help = cmd.to_string();

    // EXPECT_EQ("vier uint8:unsigned char", help);
}

// https://stackoverflow.com/questions/45502322/default-template-specialization-with-multiple-conditions/45571010

template<int N>
struct Choice: Choice<N-1>
{};

template<> struct Choice<0>
{};

template<typename T>
std::enable_if_t<std::is_integral<T>::value>
bar(Choice<2>) {
    std::cout << "integral" << std::endl;
}

template<typename T>
std::enable_if_t<std::is_same<T, std::string>::value>
bar(Choice<1>) {
    std::cout << "string" << std::endl; 
}

template<typename T>
void bar(Choice<0>) {
    std::cout << "whatever" << std::endl; 
}

template<typename T, typename... Args>
void foo(Args&&... args) { 
    bar<T>(Choice<100>{}); 
}

TEST(SmackCliTest, TemplateTest) {
    using smack::cli::Commands;

    foo<bool>();
    foo<std::string>();
    foo<void>();
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
