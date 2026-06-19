/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Example for a localized cli help page.
 *
 * Copyright © 2026 Michael Binz
 */

#include <iostream>
#include <string>

#include <smack_cli.hpp>

#include <rb_cli_localized.h>

namespace {

using smack::cli::Commands;

int add(float p1, float p2) {
    std::cout << p1 << " + " << p2 << " = " << (p1 + p2) << std::endl;
    return EXIT_SUCCESS;
}
int subtract(float p1, float p2) {
    std::cout << p1 << " - " << p2 << " = " << (p1 - p2) << std::endl;
    return EXIT_SUCCESS;
}
int multiply(float p1, float p2) {
    std::cout << p1 << " * " << p2 << " = " << (p1 * p2) << std::endl;
    return EXIT_SUCCESS;
}
int divide(float p1, float p2) {
    std::cout << p1 << " / " << p2 << " = " << (p1 / p2) << std::endl;
    return EXIT_SUCCESS;
}

} // namespace anonymous

int main(int argc, char**argv) {

    smack::cli::CliApplication cli(
        Commands::make<add>(
            "add",
            smack::rb_cli_localized.tl("doc.plus"),
            { "p1", "p2" }),
        Commands::make<subtract>(
            "subtract",
            smack::rb_cli_localized.tl("doc.minus"),
            { "p1", "p2" }),
        Commands::make<multiply>(
            "multiply",
            smack::rb_cli_localized.tl("doc.times"),
            { "p1", "p2" }),
        Commands::make<divide>(
            "divide",
            smack::rb_cli_localized.tl("doc.divide"),
            { "p1", "p2" })
    );

    return cli.launch(argc, argv);
}
