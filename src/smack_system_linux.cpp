/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * System utilities - Linux/POSIX implementation.
 *
 * Copyright © 2026 Michael Binz
 */

#include "smack_locale.h"

#include <locale>
#include <string>

namespace smack::localisation {

/**
 * Returns the system locale by querying the POSIX locale settings.
 * If no locale information is available, the default locale is returned.
 *
 * The POSIX locale string has the format:
 *   language[_territory][.codeset][@modifier]
 * e.g. "de_DE.UTF-8" or "en_US.UTF-8".
 */
auto getSystemLocale() -> Locale
{
    // Query the system's default locale name.
    const std::string raw = std::locale("").name();

    // No usable locale information available — return the default locale.
    if ( raw.empty()
            || raw == "C"
            || raw == "POSIX" )
        return Locale{};

    std::string loc{ raw };

    // Strip codeset (.UTF-8 etc.) and modifier (@euro etc.).
    auto dot = loc.find( '.' );
    if ( dot != std::string::npos )
        loc.erase( dot );

    auto at = loc.find( '@' );
    if ( at != std::string::npos )
        loc.erase( at );

    // Split on '_' to separate language from territory.
    auto sep = loc.find( '_' );
    if ( sep == std::string::npos )
        return Locale{ loc };

    return Locale{ loc.substr( 0, sep ), loc.substr( sep + 1 ) };
}

} // namespace smack::localisation
