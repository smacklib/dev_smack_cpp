/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * The smack test cli.
 *
 * Copyright © 2026 Michael Binz
 */

#include <iostream>
#include <string>

#include <smack_cli.hpp>
#include <smack_common.h>
#include <smack_locale.h>

// The generated resource bundle header.
#include "rb_smack.h"

namespace {

using smack::cli::Commands;

int getLocale() {
    std::cout << "Locale: " << std::locale("").name() << std::endl;
    std::cout << "Smack Locale: " << smack::Locale::getCurrent().toString() << std::endl;
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

int cmdInfo()
{
    std::cout << smack::rb_smack.toString() << std::endl;

    return EXIT_SUCCESS;
}

int cmdTranslate1( std::string key )
{
    std::cerr <<
        "Using system locale '" <<
        smack::Locale::getCurrent().toString() <<
        "'.\n";

    std::cout << smack::rb_smack.tl(key) << std::endl;

    return EXIT_SUCCESS;
}

int cmdTranslate2( const std::string& key, const std::string& slocale )
{
    auto locale = smack::Locale::makeLocaleFromName(slocale);

    if ( ! smack::rb_smack.hasDefinitions(locale) ) {
        std::cerr <<
            "Warning: No definitions for locale '" <<
            slocale <<
            "'.\n";
    }

    smack::Locale::setCurrent(locale);

    smack::rb_smack.hasDefinitions(smack::Locale::getCurrent());
    std::cout << smack::rb_smack.tl(key) << std::endl;

    return EXIT_SUCCESS;
}


} // namespace anonymous

int main( int argc, char** argv) {

    smack::cli::CliApplication cli(
        Commands::make<cmdInfo>(
            "info",
            "Print resource bundle information."
            ),
        Commands::make<cmdTranslate1>(
            "translate",
            "Use the resource bundle with the given key and the system locale.",
            {"key"}),
        Commands::make<cmdTranslate2>(
            "translate",
            "Use the resource bundle with the given key and locale.",
            {"key", "locale"}),
        Commands::make<getLocale>(
            "locale"),
        Commands::make<version>(
            "version")
    );

    return cli.launch(argc, argv);
}
