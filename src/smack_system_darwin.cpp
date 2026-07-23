/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * System utilities - macOS implementation.
 *
 * Copyright © 2026 Michael Binz
 */

 #include <cxxabi.h>
 #include <string>
 #include <vector>

 #include "smack_system.h"

#include <mach-o/dyld.h>
#include <CoreFoundation/CoreFoundation.h>

namespace smack::system {

using smack::Locale;

auto demangle(const char* name) -> std::string
{
    int status{0};

    // abi::__cxa_demangle returns a malloc()ed ptr.
    // std::unique_ptr uses std::free() as a user-defined Deleter.
    // Guru knowledge, rarely used.
    std::unique_ptr<char, decltype(&std::free)> result{
        abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};

    return (status == 0) ? result.get() : name;
}

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

auto executablePath() -> std::filesystem::path
{
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    _NSGetExecutablePath(buf.data(), &size);

    return std::filesystem::canonical(buf);
}

} // namespace smack::system
