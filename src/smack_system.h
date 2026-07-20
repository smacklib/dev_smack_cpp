/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * System utilities.
 *
 * Copyright © 2026 Michael Binz
 */

#pragma once

#include "smack_locale.h"
#include <string>
#include <typeinfo>
#include <type_traits>

namespace smack::system
{
auto demangle(const char* name) -> std::string;

/**
 * Demangles a C++ mangled type T.
 *
 * @tparam T The type to demangle.
 * @return The demangled name or the passed name in case demangling failed.
 */
template<typename T>
auto demangle() -> std::string
{
    return demangle(typeid(T).name());
}

/**
 * Returns the system locale by querying the underlying operating system.
 */
auto getLocale() -> smack::Locale;

} // namespace smack::system
