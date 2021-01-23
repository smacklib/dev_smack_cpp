/*
 * Copyright © 2019 Daimler TSS
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "test_common.hpp"

#include "../src/util_console_app.hpp"

static int free_function(int p1) {
    std::cout <<
        __func__ <<
        " : " <<
        p1 <<
        std::endl;
    return EXIT_SUCCESS;
}

class TestApplication
{
    std::string name_;

public:
    TestApplication(std::string name) :
        name_(name) {
    }

    // Ensures that we do not accidentally copy the host application.
    TestApplication(const TestApplication&) = delete;
    TestApplication& operator=(const TestApplication&) = delete;

    int f1(int p1) {
        return tss::test::common::f(__func__, p1);
    }

    int f2(int p1, const char* p2) {
        return tss::test::common::f(__func__, p1, p2);
    }

    int f3(int p1, double p2, const char* p3) {
        return tss::test::common::f(__func__, p1, p2, p3);
    }

    int f4(const std::string p1) {
        return tss::test::common::f(__func__, p1);
    }

    int f5(const std::string& p1) {
        return tss::test::common::f(__func__, p1);
    }

    int f6(bool p1) {
        return tss::test::common::f(__func__, p1);
    }

    int f7(int p1) const {
        return tss::test::common::f(__func__, p1);
    }
    int f7_2(int p1, double p2) const {
        return tss::test::common::f(__func__, p1, p2);
    }

    int execute(const std::vector<std::string>& argv) {
        auto cmd1 = smack::util::Commands<
            int>::make(
            "eins",
            *this,
            &TestApplication::f1);

        auto cmd2 = smack::util::Commands<
            int,
            const char*>::make(
                "zwei",
                *this,
                &TestApplication::f2);

        auto cmd3 = smack::util::Commands<
            int,
            double,
            const char*>::make(
                "drei",
                *this,
                &TestApplication::f3);

        auto cmd4 = smack::util::Commands<
            std::string>::make(
                "vier",
                *this,
                &TestApplication::f4);

        auto cmd5 = smack::util::Commands<
            const std::string&>::make(
                "fuenf",
                *this,
                &TestApplication::f5);

        auto cmd6 = smack::util::Commands<
            bool>::make(
                "sechs",
                *this,
                &TestApplication::f6);

        auto cmd7 = smack::util::Commands<
            int> ::make(
                "sieben",
                *this,
                &TestApplication::f7);

        auto cmd7_2 = smack::util::Commands<
            int,double>::make(
                "sieben",
                *this,
                &TestApplication::f7_2);

        auto cmd8 = smack::util::Commands<
            int>::make(
                "acht",
                free_function
            );

        auto cli = smack::util::makeCliApplication(
            cmd1,
            cmd2,
            cmd3,
            cmd4,
            cmd5,
            cmd6,
            cmd7,
            cmd7_2,
            cmd8);

        return cli.launch(argv);
    }
};

int main(int argc, char**argv)
{
    TestApplication ta{ 
        argv[0] 
    };

    std::vector<std::string> cmdArgv(
        argv + 1, 
        argv + argc);

    return ta.execute(cmdArgv);
}
