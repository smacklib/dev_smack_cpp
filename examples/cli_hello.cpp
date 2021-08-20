/*
 * Smack C++
 *
 * Copyright © 2019 Michael Binz
 */

#include <iostream>
#include <vector>
#include <string>

// Include smack cli support.
#include <smack_cli.hpp>

namespace {

using smack::cli::Commands;
using std::cout;
using std::endl;
using std::string;

// Offer the functionality to be called from the command line
// in typed functions.  These are only called if the parameters
// could be converted successfully to the required type.
// The function's return value represents the process return code.

int add_int(int p1, int p2) {
    cout << (p1+p2) << endl;
    return EXIT_SUCCESS;
}

int add_float(double p1, double p2) {
    cout << (p1+p2) << endl;
    return EXIT_SUCCESS;
}

int concat(string p1, string p2) {
    cout << (p1+p2) << endl;
    return EXIT_SUCCESS;
}

// No arguments.
int pi() {
    cout << 3.14159265 << endl;
    return EXIT_SUCCESS;
}

// Throws an error.  Demonstrates error handling.
int error() {
    throw std::runtime_error( "error() was called." );
}

} // namespace anonymous

int main(int argc, char**argv) {

    // Create an application proxy and register the
    // commands with the proxy. 
    smack::cli::CliApplication cli{
        "Demonstrates smack::cli.  Enjoy...",

        Commands::make<add_int>(
            // The command name that is used to select the
            // command on the command line.  This can be freely
            // selected.
            "add_int", "Adds two integers.",
            // An optional list of symbolic argument names.  This
            // is printed in the cli help page.
            { "first", "second" }),

        Commands::make<add_float>(
            "add_float", "Adds two floating point numbers."),

        Commands::make<concat>(
            "concat", "Concatenates strings." ),

        Commands::make<pi>(
            "pi", "Returns PI."),

        Commands::make<error>(
            "error", "Prints an error.")
    };

    // Finally launch the application.
    return cli.launch(argc, argv);
}
