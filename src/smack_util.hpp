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

namespace smack::util {

using std::string;
using std::vector;

namespace strings {
/**
 * Split a string at delimiter positions.
 * @param in The string to split. If this is empty then the result
 * vector is empty.
 * @param delimiter The delimiter. If this is empty then the input
 * string is returned as single token in the result.
 */
vector<string> split(const string& in, const string& delimiter);

string concat(const vector<string>& in, const string& delimiter);

template <typename T>
string trim_(const string& in, T pred)
{
    size_t begin = 0;
    while (begin < in.length() && pred(in[begin]))
        begin++;
    size_t end = in.length();
    while (end > 0 && pred(in[end - 1]))
        end--;
    if (end <= begin)
        return string{};
    return in.substr(begin, end - begin);
}

string trim(const string& in);

string trim(const string& in, const string& toTrim);

inline bool starts_with( const string& in, const string& prefix )
{
    return in.rfind( prefix, 0 ) != string::npos;
}

inline bool ends_with( const string& in, const string& suffix )
{
    return in.find( suffix, in.length() - suffix.length() ) != string::npos;
}

} // namespace strings

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
     */
    // Requires C++17 Class template argument deduction.
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

} // namespace smack::util
