/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * System utilities - macOS implementation.
 *
 * Copyright © 2026 Michael Binz
 */

#include "smack_locale.h"

#include <CoreFoundation/CoreFoundation.h>

#include <string>
#include <vector>

namespace smack::system {

using smack::Locale;

/**
 * Returns the system locale by querying the Core Foundation locale APIs.
 * If no locale information is available, the default locale is returned.
 *
 * CFLocaleCopyCurrent() returns the locale preferred by the user.
 * The locale identifier has the form "de_DE" or "en_US" (language_TERRITORY).
 */
auto getLocale() -> Locale
{
    CFLocaleRef cfLocale = CFLocaleCopyCurrent();
    if ( cfLocale == nullptr )
        return Locale{};

    CFStringRef cfId = CFLocaleGetIdentifier( cfLocale );

    // Convert the CFString to a std::string (UTF-8).
    CFIndex len = CFStringGetLength( cfId );
    CFIndex maxSize = CFStringGetMaximumSizeForEncoding( len, kCFStringEncodingUTF8 ) + 1;

    std::vector<char> buf( static_cast<std::size_t>( maxSize ) );
    std::string loc;

    if ( CFStringGetCString( cfId, buf.data(), maxSize, kCFStringEncodingUTF8 ) )
        loc = buf.data();

    CFRelease( cfLocale );

    if ( loc.empty() )
        return Locale{};

    // The identifier uses '_' as separator, e.g. "de_DE".
    auto sep = loc.find( '_' );
    if ( sep == std::string::npos )
        return Locale{ loc };

    return Locale{ loc.substr( 0, sep ), loc.substr( sep + 1 ) };
}

} // namespace smack::system
