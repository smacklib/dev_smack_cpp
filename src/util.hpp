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
 * Resource management with explict non-destrucor-based release.
 */
template <typename T, typename C>
class Disposer
{
    Disposer(const Disposer&) = delete;
private:
    T cleanupArg_;
    C cleanup_;
    bool released_;

public:
    Disposer(T arg, C c) : released_(false)
    {
        cleanupArg_ = arg;
        cleanup_ = c;
    }

    Disposer(Disposer&&) = default;

    ~Disposer()
    {
        if (released_)
            return;

        cleanup_(cleanupArg_);
    }

    void release()
    {
        cleanup_(cleanupArg_);
        released_ = true;
    }

    // Auto convert to T.
    T handle()
    {
        return cleanupArg_;
    }

    operator T() const
    {
        return cleanupArg_;
    }
};

} // namespace util
} // namespace smack
