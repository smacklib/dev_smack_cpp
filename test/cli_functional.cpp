/*
 * Copyright © 2019 Daimler TSS
 */

#include <iostream>
#include <vector>
#include <string>

#include "test_common.hpp"

#include "../src/util_console_app.hpp"

static int f1(int p1) {
    return tss::test::common::f(__func__, p1);
}

static int f2(int p1, const char* p2) {
    return tss::test::common::f(__func__, p1, p2);
}

static int f3(int p1, double p2, const char* p3) {
    return tss::test::common::f(__func__, p1, p2, p3);
}

static int f4(const std::string p1) {
    return tss::test::common::f(__func__, p1);
}

static int f5(const std::string& p1) {
    return tss::test::common::f(__func__, p1);
}

static int f6(bool p1) {
    return tss::test::common::f(__func__, p1);
}

static int f7(int p1) {
    return tss::test::common::f(__func__, p1);
}
static int f7(int p1, double p2) {
    return tss::test::common::f(__func__, p1, p2);
}

static int execute(const std::vector<std::string>& argv) {
    auto cmd1 = tss::util::Commands<
        int>::make(
        "eins",
        f1);

    auto cmd2 = tss::util::Commands<
        int,
        const char*>::make(
            "zwei",
            f2);

    auto cmd3 = tss::util::Commands<
        int,
        double,
        const char*>::make(
            "drei",
            f3);

    auto cmd4 = tss::util::Commands<
        std::string>::make(
            "vier",
            f4);

    auto cmd5 = tss::util::Commands<
        const std::string&>::make(
            "fuenf",
            f5);

    auto cmd6 = tss::util::Commands<
        bool>::make(
            "sechs",
            f6);

    // Example for an overloaded function.
    auto cmd7 = tss::util::Commands<
        int> ::make(
            "sieben",
            static_cast<int(*)(int)>(f7));

    // Example for an overloaded function. Generally this is doable but
    // does not make a lot of sense.  The alternative, a different name for
    // the overloaded function, is much simpler.
    auto cmd7_2 = tss::util::Commands<
        int,double>::make(
            "sieben",
            static_cast<int(*)(int,double)>(f7));

    auto cli = tss::util::makeCliApplication(
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

int main(int argc, char**argv) {
    std::cout << argv[0] << std::endl;

    std::vector<std::string> cmdArgv(
        argv + 1, 
        argv + argc);

    return execute(cmdArgv);
}
