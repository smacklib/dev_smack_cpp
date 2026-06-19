/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * System utilities - Windows implementation.
 *
 * Copyright © 2026 Michael Binz
 */

#include "smack_locale.h"

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace smack::system {

using smack::localisation::Locale;

/**
 * Returns the system locale by querying the Windows locale APIs.
 * If no locale information is available, the default locale is returned.
 *
 * GetUserDefaultLocaleName returns a BCP-47-style tag such as "de-DE" or
 * "en-US".  The language and territory are separated by '-', which we map
 * directly to smack::localisation::Locale(language, country).
 */
auto getLocale() -> Locale
{
    wchar_t buf[LOCALE_NAME_MAX_LENGTH]{};

    // Returns the number of characters written (including the NUL terminator),
    // or 0 on failure.
    if ( GetUserDefaultLocaleName( buf, LOCALE_NAME_MAX_LENGTH ) == 0 )
        return Locale{};

    // Convert wide string to narrow UTF-8.
    int needed = WideCharToMultiByte(
        CP_UTF8, 0, buf, -1, nullptr, 0, nullptr, nullptr );
    if ( needed <= 1 )
        return Locale{};

    std::string loc( static_cast<std::size_t>( needed - 1 ), '\0' );
    WideCharToMultiByte(
        CP_UTF8, 0, buf, -1, loc.data(), needed, nullptr, nullptr );

    // BCP-47 uses '-' as separator; split into language and territory.
    auto sep = loc.find( '-' );
    if ( sep == std::string::npos )
        return Locale{ loc };

    return Locale{ loc.substr( 0, sep ), loc.substr( sep + 1 ) };
}

} // namespace smack::system
