/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 * 
 * smack::cli demo
 * 
 * Copyright © 2019-2021 Michael Binz
 */

#include <cmath>
#include <iostream>
#include <vector>
#include <string>

// Include smack cli support.
#include <smack_cli.hpp>

// This definition of type Point and the get_typename()- and transform()-
// operations below add a new type 'Point' to the automatic type conversion.
// This is required by of the 'distance' command that we offer.
using Point = std::pair<float, float>;

// Add a decent name for our new type.
template<>
constexpr const char* smack::convert::get_typename(Point type) {
    return "point";
}
// A point is written as 'x:y', e.g. '1.0:2.5'.  This operation adds
// the converter to smack::cli's automatic type transformations.
template<> void smack::convert::transform(const char* in, Point& out) {
    string input{ in };
    string delimiter{ ":" };

    auto pos = input.find(delimiter);

    if (pos == string::npos)
        throw std::invalid_argument(in);

    auto first =
        input.substr(0, pos);
    auto second =
        input.substr(pos + delimiter.length());

    transform(
        first.c_str(),
        out.first);
    transform(
        second.c_str(),
        out.second);
}

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

int dist(Point a, Point b)
{
    float result = std::hypot(
        a.first - b.first,
        a.second - b.second);

    cout << result << endl;

    return 0;
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
// Throws an error.  Demonstrates error handling.
int error_1(const string& message) {
    throw std::runtime_error(message);
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
            "add_int", "Add two integers.",
            // An optional list of symbolic argument names.  This
            // is printed in the cli help page.
            { "first", "second" }),

        Commands::make<add_float>(
            "add_float", "Add two floating point numbers."),

        Commands::make<concat>(
            "concat", "Concatenate strings." ),

        Commands::make<pi>(
            "pi", "Returns PI."),

        Commands::make<error>(
            "error", "Print a predefined error."),
        // Overload the 'error' command with a function taking one parameter. 
        Commands::make<error_1>(
            "error", "Print an error message.",
            {"message"}),

        // Add an operation that takes the Point types we defined above, see
        // 'using Point = ...' et al.  Note that this does not differ from
        // the other operations we added.  
        Commands::make<dist>(
            "distance", "Compute the distance between two points.",
            {"a", "b"})
    };

    // Finally launch the application.
    return cli.launch(argc, argv);
}
