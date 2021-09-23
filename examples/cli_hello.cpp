/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 * 
 * smack::cli example.
 * 
 * Copyright © 2019-2021 Michael Binz
 */

#include <iostream>
#include <string>

// Include smack cli support.
#include <smack_cli.hpp>

// smack::cli's basic concept is to make functions implemented in
// C++ callable from the command line as commands with minimial
// efforts for the coder.  That is, parameter conversion is
// handled automatically and a nice help page is printed if
// the cli is executed without any arguments.

// See ...

// This anonymous name space is used to hold the functions that should
// be invoked from the command line. Not needed but recommended.
namespace {

using smack::cli::Commands;
using std::cout;
using std::endl;
using std::string;

// Here come the plain functions.  Each one represents later a 
// command on the command line.
// Add function parameters as needed, these are automatically
// converted by smack::cli when the function is called from the
// command line.  Below we only use primitive parameters.
// See https://github.com/smacklib/dev_smack_cpp/blob/master/examples/cli_hello_new_type.cpp
// for an example that adds user-defined types.
// The function's return value represents the process return code.

int add_int(int p1, int p2) {
    cout << (p1+p2) << endl;
    return EXIT_SUCCESS;
}

int add_float(float p1, float p2) {
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

// Throws an error.  The error is catched by cli::smack
// and returned as a proper error message.
int error() {
    throw std::runtime_error( "error() was called." );
}
// Throws an error.  Demonstrates error handling.
int error_1(const string& message) {
    throw std::runtime_error(message);
}

} // namespace anonymous

int main(int argc, char**argv) 
{
    // Now tie everything together:  Create an application object
    // and register the commands one by one. 
    smack::cli::CliApplication cli{
        // An overall help text for the cli.
        "Demonstrates smack::cli.  Enjoy...",

        // A fully specified command.  The template parameter
        // refers to the function above that is called by the
        // command.
        Commands::make<add_int>(
            // The command name that is used to select the
            // command on the command line.  This can be freely
            // selected.
            "add_int",
            // Text describing the command.
            "Add two integers.",
            // A list of symbolic argument names.
            { "first", "second" }),

        // A minimal command, just a function reference and the 
        // command name need to be passed.
        Commands::make<add_float>(
            "add_float"),

        // Simple.  Function arguments are strings, but these
        // are automatically parsed as the arguments of the
        // functions above.
        Commands::make<concat>(
            "concat", "Concatenate strings." ),

        // No argument at all.
        Commands::make<pi>(
            "pi", "Returns PI."),

        // Overloading.  This is 'error' with no parameter.
        Commands::make<error>(
            "error", "Print a predefined error."),
        // Overload the 'error' command with a function taking one
        // parameter. 
        Commands::make<error_1>(
            "error", "Print an error message.",
            {"message"})
    };

    // Finally launch the application.  The launch operation
    // returns the executed command's return value.  We simply return
    // this from main() and are done.
    return cli.launch(argc, argv);

    // Easy, eh?
}
