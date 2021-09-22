#include <gtest/gtest.h>

#include <limits.h>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#include "test_common.hpp"
#include "smack_cli.hpp"

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

    int fError(string p1) {
        throw std::invalid_argument( p1 );
    }
}

TEST(SmackCli, CommandHelpFull) {
    using smack::cli::Commands;

    auto cmd1 = Commands::make<f1>(
        "eins",
        { "p1" });

    auto help = cmd1.to_string();

    EXPECT_EQ( "eins p1:int", help);
}

TEST(SmackCli, CommandHelpNone) {
    using smack::cli::Commands;

    auto cmd = Commands::make<f2>(
        "zwei");

    auto help = cmd.to_string();

    EXPECT_EQ("zwei int string", help);
}

TEST(SmackCli, CommandHelpPartial) {
    using smack::cli::Commands;

    auto cmd = Commands::make<f3>(
        "drei",
        { "p1", "p2" });

    auto help = cmd.to_string();

    EXPECT_EQ("drei p1:int p2:double string", help);
}

template<>
constexpr const char* smack::convert::get_typename( std::pair<int,int> type ) { 
    return "pair"; 
}


TEST(SmackCli, PairTypename) {
    using smack::convert::get_typename;

    std::pair<int,int> pair_;

    EXPECT_EQ("pair", string{ get_typename(pair_) });
}

template<> void smack::convert::transform(const char* in, std::pair<int,int>& out) {
    string input{ in };
    string delimiter{ ":" };

    auto pos = input.find( delimiter );

    if ( pos == string::npos )
        throw std::invalid_argument( in );

    auto first = 
        input.substr( 0, pos );
    auto second = 
        input.substr( pos + delimiter.length() );

    smack::convert::transform(
        first.c_str(),
        out.first );
    smack::convert::transform(
        second.c_str(),
        out.second );
}

TEST(SmackCli, CommandPairHelp) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    auto help = cmd.to_string();

    EXPECT_EQ("fPair p1:pair", help);

    std::vector<string> argv{"212:313"};

    // Redirect stdout.
    smack::test::common::redir r{ std::cout };

    cmd.callv( argv );
    // Get stout content.
    std::string text = r.str();

    EXPECT_EQ("fPair( 212, 313 )\n", text);
}

TEST(SmackCli, CommandPairExec) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    // Redirect stdout.
    smack::test::common::redir r{ std::cout };

    // Execute the command.
    cmd.call("212:313");

    // Get stout content.
    std::string text = r.str();

    EXPECT_EQ("fPair( 212, 313 )\n", text);
}

TEST(SmackCli, CommandPairExecCli) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    std::vector<string> argv{
        cmd.name(),
        "212:313" };

    smack::cli::CliApplication cli(
        cmd
    );

    smack::test::common::redir r{ std::cout };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_SUCCESS, exitCode);

    // Get stdout content.
    std::string text = r.str();

    EXPECT_EQ("fPair( 212, 313 )\n", text);
}

TEST(SmackCli, CommandCall) {
    using smack::cli::Commands;

    auto cmd = Commands::make<f3>(
        "f3");

    EXPECT_EQ("f3", cmd.name());

    string expected{"f3( 313, 3.1415, micbinz )\n"};
    {
        smack::test::common::redir out( std::cout );
        cmd.call( "313", "3.1415", "micbinz" );
        EXPECT_EQ(expected, out.str());
    }
    {
        std::vector<string> argv{"313", "3.1415", "micbinz"};
        smack::test::common::redir out( std::cout );
        cmd.callv( argv );
        EXPECT_EQ(expected, out.str());
    }
}

TEST(SmackCli, CliErrorCommandNotFound) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    smack::cli::CliApplication cli(
        cmd
    );

    smack::test::common::redir r{ std::cerr };

    std::vector<string> argv{
        "bogus",
        "hogus" };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_FAILURE, exitCode);

    // Get err content.
    auto lines = r.strs();

    EXPECT_LE(1, lines.size());
    EXPECT_EQ("Unknown command 'bogus'.", lines[0]);
}

TEST(SmackCli, CliErrorCommandArgMismatch) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fPair>(
        "fPair",
        { "p1" });

    smack::cli::CliApplication cli(
        cmd
    );

    smack::test::common::redir r{ std::cerr };

    std::vector<string> argv{
        cmd.name(),
        "212:313",
        "donald" };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_FAILURE, exitCode);

    // Get err content.
    auto lines = r.strs();

    EXPECT_LE(1, lines.size());
    EXPECT_EQ("The command 'fPair' does not support 2 parameters.", lines[0]);
}

TEST(SmackCli, CliErrorCommandException) {
    using smack::cli::Commands;

    auto cmd = Commands::make<fError>(
        "xxx",
        { "p1" });

    smack::cli::CliApplication cli(
        cmd
    );

    smack::test::common::redir r{ std::cerr };

    std::vector<string> argv{
        cmd.name(),
        "Groan!" };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_FAILURE, exitCode);

    // Get err content.
    auto lines = r.strs();

    EXPECT_LE(1, lines.size());
    EXPECT_EQ("'xxx' failed: Groan!", lines[0]);
}

TEST(SmackCli, CliTestHelp) {
    using smack::cli::Commands;

    smack::cli::CliApplication cli(
        Commands::make<fError>(
            "xxx",
            { "p1" })
    );

    cli.set_name(
        smack::test::common::g_gtestExecutableName);

    smack::test::common::redir r{ std::cerr };

    std::vector<string> argv;

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_FAILURE, exitCode);

    // Get err content.
    auto lines = r.strs();

    ASSERT_EQ(5, lines.size());
    ASSERT_EQ("Usage: smack_cpp_test COMMAND arguments", lines[0]);
    ASSERT_EQ("", lines[1]);
    ASSERT_EQ("Commands:", lines[2]);
    ASSERT_EQ("xxx p1:string", lines[3]);
    ASSERT_EQ("", lines[4]);
}

TEST(SmackCli, CliTestHelpExplicit) {
    using smack::cli::Commands;

    string applicationHelpString{ "application help string." };

    smack::cli::CliApplication cli(
        applicationHelpString.c_str(),
        Commands::make<fError>(
            "xxx",
            { "p1" })
    );

    cli.set_name(
        smack::test::common::g_gtestExecutableName);

    smack::test::common::redir r{ std::cout };

    std::vector<string> argv{ "?" };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_SUCCESS, exitCode);

    // Get err content.
    auto lines = r.strs();

    ASSERT_EQ(6, lines.size());
    ASSERT_EQ("Usage: smack_cpp_test COMMAND arguments", lines[0]);
    ASSERT_EQ(applicationHelpString, lines[1]);
    ASSERT_EQ("", lines[2]);
    ASSERT_EQ("Commands:", lines[3]);
    ASSERT_EQ("xxx p1:string", lines[4]);
    ASSERT_EQ("", lines[5]);
}

TEST(SmackCli, CliTestHelpSorted) {
    using smack::cli::Commands;

    smack::cli::CliApplication cli(
        Commands::make<fError>("zzz"),
        Commands::make<fError>("xxx"),
        Commands::make<fError>("aaa")
    );

    cli.set_name(
        smack::test::common::g_gtestExecutableName);

    smack::test::common::redir r{ std::cout };

    std::vector<string> argv{ "?" };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_SUCCESS, exitCode);

    // Get err content.
    auto lines = r.strs();

    ASSERT_EQ(7, lines.size());
    ASSERT_EQ("Commands:", lines[2]);
    ASSERT_EQ("aaa string", lines[3]);
    ASSERT_EQ("xxx string", lines[4]);
    ASSERT_EQ("zzz string", lines[5]);
    ASSERT_EQ("", lines[6]);
}

TEST(SmackCli, CliTestCommandDescription) {
    using smack::cli::Commands;

    string applicationHelpString{ "application help string." };

    smack::cli::CliApplication cli(
        applicationHelpString.c_str(),
        Commands::make<fError>(
            "xxx", 
            "description:xxx",
            { "p1" })
    );

    cli.set_name(
        smack::test::common::g_gtestExecutableName);

    smack::test::common::redir r{ std::cout };

    std::vector<string> argv{ "?" };

    // Execute the application.
    auto exitCode =
        cli.launch(argv);

    EXPECT_EQ(EXIT_SUCCESS, exitCode);

    // Get err content.
    auto lines = r.strs();

    ASSERT_EQ(7, lines.size());
    ASSERT_EQ("Usage: smack_cpp_test COMMAND arguments", lines[0]);
    ASSERT_EQ(applicationHelpString, lines[1]);
    ASSERT_EQ("", lines[2]);
    ASSERT_EQ("Commands:", lines[3]);
    ASSERT_EQ("xxx p1:string", lines[4]);
    ASSERT_EQ("    description:xxx", lines[5]);
    ASSERT_EQ("", lines[6]);
}

TEST(SmackCli, TemplateCtor) {
    using namespace std::string_literals;

    std::tuple<int,const char *> bah;
    smack::cli::Command c { 
        "313"s,
        "desc", {"hu", "ha"}, bah, f2 };
}
