/*
 * Smack C++
 *
 * Copyright © 2019 Michael Binz
 */

#include <iostream>
#include <vector>
#include <string>

#include "../test/test_common.hpp"

#include <smack_cli.hpp>

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

} // namespace anonymous

int main(int argc, char**argv) {
    std::cout << argv[0] << std::endl;

    auto cli = smack::cli::makeCliApplication(
        Commands::make<f1>(
            "eins",
            { "p1" }),

        Commands::make<f2>(
            "zwei"),

        Commands::make<f3>(
            "drei"),

        Commands::make<f4>(
            "vier"),

        Commands::make<f5>(
            "fuenf"),

        Commands::make<f6>(
            "sechs"),

        // Example for an overloaded function.
        Commands::make<static_cast<int(*)(int)>(f7)>(
            "sieben"),

        // Example for an overloaded function. Generally this is doable but
        // does not make a lot of sense.  The alternative, a different name for
        // the overloaded function, is much simpler.
        Commands::make<static_cast<int(*)(int, double)>(f7)>(
            "sieben_2")
    );

    return cli.launch(argc, argv);
}
