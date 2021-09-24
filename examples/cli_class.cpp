/*
 * Smack C++
 *
 * Copyright © 2019 Michael Binz
 */

#include <iostream>
#include <string>

#include "../test/test_common.hpp"

#include <smack_cli.hpp>

using std::cout;
using std::endl;

namespace {
int free_function(int p1) {
    std::cout <<
        __func__ <<
        " : " <<
        p1 <<
        std::endl;
    return EXIT_SUCCESS;
}
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

    int f7(int p1) const {
        return smack::test::common::f(__func__, p1);
    }

    int f7_2(int p1, double p2) const {
        return smack::test::common::f(__func__, p1, p2);
    }

    int execute(int argc, char **argv) {
        using smack::cli::Commands;

        smack::cli::CliApplication cli(
            Commands::make<&TestApplication::f1>(
                "eins", "description:eins", this),
            Commands::make<&TestApplication::f2>(
                "zwei", this),
            Commands::make<&TestApplication::f3>(
                "drei", this),
            Commands::make<&TestApplication::f4>(
                "vier", this),
            Commands::make<&TestApplication::f5>(
                "fuenf", this),
            Commands::make<&TestApplication::f6>(
                "sechs", this),

            // Register two commands with argument lists of different
            // length for the same command name.
            Commands::make<&TestApplication::f7>(
                "sieben", this),
            Commands::make<&TestApplication::f7_2>(
                "sieben", this ),

            Commands::make<free_function>(
                "acht") );

        return cli.launch(argc,argv);
    }
};

int main(int argc, char**argv)
{
    TestApplication ta{ 
        argv[0] 
    };

    return ta.execute(argc, argv);
}
