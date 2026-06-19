/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * The smack cli.
 *
 * Copyright © 2026 Michael Binz
 */

#include <iostream>
#include <string>

#include <smack_cli.hpp>
#include <smack_common.h>
#include <smack_locale.h>

namespace {

using smack::cli::Commands;

int getLocale() {
    std::cout << "Locale: " << std::locale("").name() << std::endl;
    std::cout << "Smack Locale: " << smack::localisation::getSystemLocale().toString() << std::endl;
    return EXIT_SUCCESS;
}

int version() {
    std::cout <<
        smack::version.major <<
        "." <<
        smack::version.minor <<
        "." <<
        smack::version.patch << std::endl;

    return EXIT_SUCCESS;
}

} // namespace anonymous

int main( int argc, char** argv) {

    smack::cli::CliApplication cli(
        Commands::make<getLocale>(
            "locale"),
        Commands::make<version>(
            "version")
    );

    return cli.launch(argc, argv);
}
