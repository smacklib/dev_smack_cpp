/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Implements the command for resource bundle generation.
 *
 * Copyright © 2026 Michael Binz
 */

#include <iostream>
#include <string>

#include <smack_cli.hpp>
#include <smack_common.h>
#include <smack_locale.h>
#include <smack_properties.hpp>
#include "smack_gen.hpp"

namespace smack::cli
{

int cmdUsePrototype0()
{
    std::cout << smack::rb_smack.toString() << std::endl;

    return EXIT_SUCCESS;
}

int cmdUsePrototype1( std::string key )
{
    std::cout << smack::rb_smack.tl(key) << std::endl;

    return EXIT_SUCCESS;
}

int cmdUsePrototype2( std::string country, std::string key )
{
    smack::localisation::Locale::setCurrent(smack::localisation::Locale(country));

    std::cout << smack::rb_smack.tl(key) << std::endl;

    return EXIT_SUCCESS;
}

int cmdUsePrototype3( std::string country, std::string language, std::string key )
{
    smack::localisation::Locale::setCurrent(smack::localisation::Locale(country, language));

    std::cout << smack::rb_smack.tl(key) << std::endl;

    return EXIT_SUCCESS;
}

} // namespace smack::cli
