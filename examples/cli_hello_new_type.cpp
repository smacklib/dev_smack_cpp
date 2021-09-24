/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 * 
 * smack::cli demo
 * 
 * Copyright © 2019-2021 Michael Binz
 */

#include <cmath>
#include <iostream>
#include <string>

// Include smack cli support.
#include <smack_cli.hpp>

// This example adds a new type to the smack::cli runtime system.
// This is an advanced use case.  For an example of smack::cli
// basics see https://github.com/smacklib/dev_smack_cpp/blob/master/examples/cli_hello.cpp


// This definition of type Point and the get_typename()- and transform()-
// operations below add a new type 'Point' to the smack::cli type conversion.
// This is needed by the 'distance' command we want to offer.
using Point = std::pair<float, float>;

namespace smack::convert {

// Add a decent name for our new type.
template<>
constexpr const char* get_typename(Point type) {
    return "point";
}
// A point is written as 'x:y', e.g. '1.0:2.5'.  This operation adds
// the converter to smack::cli's automatic type transformations.
template<> void transform(const char* in, Point& out) {
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

} // namespace smack::convert

namespace {

using smack::cli::Commands;
using std::cout;
using std::endl;
using std::string;

// Offer the functionality to be called from the command line
// in typed functions.  Since we added the Point type above
// the signatures can now contain arguments of this type.

int dist(Point a, Point b)
{
    float result = std::hypot(
        a.first - b.first,
        a.second - b.second);

    cout << result << endl;

    return 0;
}

// The remaining functionality continues to work unmidf
int move_x(Point p, float distance) {
    cout << (p.first + distance) << ":" << p.second << endl;
    return EXIT_SUCCESS;
}

} // namespace anonymous

int main(int argc, char**argv) 
{
    // Create an application object and register the commands. 
    smack::cli::CliApplication cli{
        "Demonstrates smack::cli with extended mapping.  Enjoy...",

        // Add an operation that takes the Point types we defined above, see
        // 'using Point = ...' et al.
        Commands::make<dist>(
            "distance", "Compute the distance between two points.",
            {"a", "b"}),

        // Add an second command that combines a primitive argument with a
        // Point.
        Commands::make<move_x>(
            "move_x", "Moves p in x direction.", {"p"})
    };

    // Finally launch the application.
    return cli.launch(argc, argv);
}
