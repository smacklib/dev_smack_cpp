/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * The smack cli.
 *
 * Copyright © 2026 Michael Binz
 */

#include <iostream>
#include <string>

#include "cmdGenerateResourceBundle.h"
#include "cmdUsePrototype.h"
#include <smack_cli.hpp>
#include <smack_common.h>
#include <smack_locale.h>

namespace {

using smack::cli::Commands;

int getLocale() {
    std::cout << "Locale: " << std::locale("").name() << std::endl;
    std::cout << "Smack Locale: " << smack::localisation::Locale::getCurrent().toString() << std::endl;
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
        Commands::make<&smack::cli::cmdGenerateResourceBundle>(
            "generate-resource-bundle",
            "Generate a resource bundle for the given base bundle file.",
            {"baseBundleFile"}),
        Commands::make<&smack::cli::cmdUsePrototype0>(
            "use-prototype",
            "Print prototype information."
            ),
        Commands::make<&smack::cli::cmdUsePrototype1>(
            "use-prototype",
            "Use the prototype with the given key.",
            {"key"}),
        Commands::make<&smack::cli::cmdUsePrototype2>(
            "use-prototype",
            "Use the prototype with the given country and key.",
            {"country", "key"}),
        Commands::make<&smack::cli::cmdUsePrototype3>(
            "use-prototype",
            "Use the prototype with the given country, language, and key.",
            {"country", "language", "key"}),
        Commands::make<getLocale>(
            "locale"),
        Commands::make<version>(
            "version")
    );

    return cli.launch(argc, argv);
}
