/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Command definitions.
 *
 * Copyright © 2026 Michael Binz
 */

#pragma once

#include <string>

namespace smack::cli
{
    auto cmdUsePrototype0() -> int;
    auto cmdUsePrototype1( std::string key ) -> int;
    auto cmdUsePrototype2( std::string country, std::string key ) -> int;
    auto cmdUsePrototype3( std::string country, std::string language, std::string key ) -> int;
} // namespace smack::cli
