/*
 * Smack C++
 *
 * Copyright © 2019 Michael Binz
 */

#include <iostream>
#include <vector>
#include <string>

#include "test_common.hpp"

#include "../src/smack_cli.hpp"

namespace {

using smack::cli::Commands;

int f1(int p1) {
    return smack::test::common::f(__func__, p1);
}

int f2(int p1, const char* p2) {
    return smack::test::common::f(__func__, p1, p2);
}

int f3(int p1, double p2, const char* p3) {
    return smack::test::common::f(__func__, p1, p2, p3);
}

int f4(const std::string p1) {
    return smack::test::common::f(__func__, p1);
}

int f5(const std::string& p1) {
    return smack::test::common::f(__func__, p1);
}

int f6(bool p1) {
    return smack::test::common::f(__func__, p1);
}

int f7(int p1) {
    return smack::test::common::f(__func__, p1);
}
int f7(int p1, double p2) {
    return smack::test::common::f(__func__, p1, p2);
}

int execute(const std::vector<std::string>& argv) {
    auto cmd1 = Commands::make<f1>(
        "eins",
        { "p1" });

    auto cmd2 = Commands::make<f2>(
        "zwei");

    auto cmd3 = Commands::make<f3>(
        "drei");

    auto cmd4 = Commands::make<f4>(
        "vier");

    auto cmd5 = Commands::make<f5>(
        "fuenf");

    auto cmd6 = Commands::make<f6>(
        "sechs");

    // Example for an overloaded function.
    auto cmd7 = Commands::make<static_cast<int(*)(int)>(f7)>(
        "sieben");

    // Example for an overloaded function. Generally this is doable but
    // does not make a lot of sense.  The alternative, a different name for
    // the overloaded function, is much simpler.
    auto cmd7_2 = Commands::make<static_cast<int(*)(int, double)>(f7)>(
        "sieben_2");

    auto cli = smack::cli::makeCliApplication(
        cmd1,
        cmd2,
        cmd3,
        cmd4,
        cmd5,
        cmd6,
        cmd7,
        cmd7_2
    );

    return cli.launch(argv);
}

} // namespace anonymous

int main(int argc, char**argv) {
    std::cout << argv[0] << std::endl;

    std::vector<std::string> cmdArgv(
        argv + 1, 
        argv + argc);

    return execute(cmdArgv);
}
