/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * General utilities.
 *
 * Copyright © 2021-2025 Michael Binz
 */

#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace smack
{

/**
 * Performs an in-place sort of the passed container.
 */
template <typename C>
auto sort( C& c ) -> void
{
    std::sort( c.begin(), c.end() );
}

} // namespace smack

namespace smack::util {

using std::string;
using std::string_view;
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

/**
 * Concatenate strings using a delimiter. The delimiter is inserted between
 * the strings, not at the end or the beginning.  An example is 
 * concat( {"a", "b", "c"}, "-" ) -> "a-b-c" ).
 * 
 * @param in The strings to concatenate.  If the container is empty the
 * empty string is returned.  If only a single string is contained this
 * is returned.
 * @param delimiter The delimiter to use.  This may be the empty string.
 */
string concat(const vector<string>& in, const string& delimiter);

/**
 * Trims, i.e. removes, space characters at the beginning and the end
 * of the passed string.  Formally characters where std::isspace() 
 * returns true are removed.
 * 
 * @param in The string to trim.
 * @returns The trimmed string.
 */
string trim(const string& in);
string_view trim(string_view in);

/**
 * Trims, i.e. removes, characters at the beginning and the end
 * of the passed string.  The characters to be removed can be
 * passed.
 *
 * @param in The string to trim.
 * @param toTrim A string containing characters to be trimmed.
 * @returns The trimmed string.
 */
string trim(const string& in, const string& toTrim);
string_view trim(string_view in, const string_view& toTrim);

/**
 * Checks if a string starts with a given prefix.
 *
 * @param in The string.
 * @param prefix The prefix.
 * @returns true if the passed string starts with the prefix,
 * false otherwise.
 */
inline constexpr bool starts_with( const string_view& in, const string_view& prefix )
{
    return in.rfind( prefix, 0 ) != string::npos;
}

/**
 * Checks if a string ends with a given suffix.
 *
 * @param in The string.
 * @param suffix The suffix.
 * @returns true if the passed string ends with the suffix,
 * false otherwise.
 */
inline constexpr bool ends_with( const string_view& in, const string_view& suffix )
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
     * before the automatic release from the destructor is called.
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
