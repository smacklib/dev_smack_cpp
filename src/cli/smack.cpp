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
#include <smack_system.h>

#include "cmdGenerateResourceBundle.h"

namespace {

using smack::cli::Commands;

int getLocale() {
    std::cout << "Locale: " << std::locale("").name() << std::endl;
    std::cout << "Smack Locale: " << smack::Locale::getCurrent().toString() << std::endl;
    return EXIT_SUCCESS;
}

int cmdSelf() {
    std::cout << "Executable Path: " << smack::system::executablePath() << std::endl;
    return EXIT_SUCCESS;
}

int version() {
    std::cout
        << smack::version.major
        << "."
        << smack::version.minor
        << "."
        << smack::version.patch
        << " "
#if defined(SMACK_LINUX)
        << "SMACK_LINUX"
#elif defined(SMACK_WINDOWS)
        << "SMACK_WINDOWS"
#elif defined(SMACK_MAC)
        << "SMACK_MAC"
#else
        << "SMACK_UNKNOWN"
#endif

    << std::endl;

    return EXIT_SUCCESS;
}

} // namespace anonymous

int main( int argc, char** argv) {

    smack::cli::CliApplication cli(
        Commands::make<smack::cli::cmdGenerateResourceBundle>(
            "generate-resource-bundle",
            "Generate a resource bundle for the given base bundle file.",
            {"baseBundleFile"}),
        Commands::make<smack::cli::cmdGenerateResourceBundleToDirectory>(
            "generate-resource-bundle",
            "Generate a resource bundle into outputDir as rb_<bundleName>.h.",
            {"baseBundleFile", "outputDir"}),
        Commands::make<getLocale>(
            "locale"),
        Commands::make<cmdSelf>(
            "self"),
        Commands::make<version>(
            "version")
    );

    return cli.launch(argc, argv);
}
