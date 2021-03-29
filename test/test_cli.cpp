#include <gtest/gtest.h> // googletest header file

#include <string>

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

TEST(SmackCliTest, TransformChar) {
    const char* in = "13";
    char out;

    smack::cli::transform(in, out);

    EXPECT_EQ(13, out);
}

TEST(SmackCliTest, TransformCharFail) {
    try {
        const char* in = "dreizehn";
        char out;

        smack::cli::transform(in, out);

        FAIL();
    }
    catch (std::invalid_argument(in))
    {
        std::string exp{ "invalid stoi argument" };
        EXPECT_EQ(exp, in.what());
    }
}
