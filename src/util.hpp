/*
 * Smack C++
 *
 * Copyright © 2021 Michael Binz
 */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <utility>

namespace smack {
namespace util {

/**
 * Resource management with explict non-destructor-based release.
 * Used primarily in interfacing to c-based stateful libraries
 * requiring resource management.
 */
template <typename T, typename C>
class Disposer
{
    Disposer(const Disposer&) = delete;
    Disposer(Disposer&&) = delete;
private:
    T cleanupArg_;
    C cleanup_;
    bool released_;

public:
    /**
     * Creates a Disposer with an handle argument and a cleanup
     * operation of cleanupOp.
     * @param arg The handle argument.
     *
     */
    Disposer(T handle, C cleanupOp) : 
        cleanupArg_( handle ),
        cleanup_( cleanupOp ),
        released_(false) {
    }

    ~Disposer() {
        dispose();
    }

    /**
     * Explicit release.  This is only needed if release is needed
     * before the aoutomatic release from the destructor is called.
     * This can be called more than once, only a single release
     * is performed.
     */
    void dispose() {
        if (released_)
            return;

        cleanup_(cleanupArg_);

        released_ = true;
    }

    /**
     * Auto convert to T, the handle type.
     */
    operator T() const noexcept {
        return cleanupArg_;
    }
};

} // namespace util
} // namespace smack
