/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Common types used in Smack.  This file is to be included by all Smack
 * modules.
 *
 * Copyright © 2026 Michael Binz
 */

#pragma once

#include <chrono>
#include <functional>

namespace smack {

    /**
     * An executable unit of code.
     */
    using THUNK = std::function<void()>;

    /**
     * A consumer (and executor) of scheduled thunks.
     */
    using CONSUMER = std::function<void(THUNK)>;

    using Duration =
        std::chrono::milliseconds;
    using TimePoint =
        std::chrono::time_point<std::chrono::system_clock>;
}

using namespace std::chrono_literals;
