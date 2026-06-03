/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * General utilities.
 *
 * Copyright © 2021-2025 Michael Binz
 */

#pragma once

#include <algorithm>
#include <atomic>
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

/**
 * Get a unique thread id, much shorter than the std::thread id.
 * Used for debugging and logging.
 * */
auto thread_id() -> unsigned;

/**
 * Split a string at delimiter positions.
 * @param in The string to split. If this is empty then the result
 * vector is empty.
 * @param delimiter The delimiter. If this is empty then the input
 * string is returned as single token in the result.
 */
std::vector<std::string> split(const std::string& in, const std::string& delimiter);

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
std::string concat(const std::vector<std::string>& in, const std::string& delimiter);

/**
 * Trims space characters at the beginning and the end of the passed string.
 * Formally characters where std::isspace() returns true are removed.
 *
 * @param in The string to trim.
 * @returns The trimmed string.
 */
std::string trim(const std::string& in);
std::string_view trim(std::string_view in);

/**
 * Trims specified characters at the beginning and the end of the passed string.
 *
 * @param in The string to trim.
 * @param toTrim A string containing characters to be trimmed.
 * @returns The trimmed string.
 */
std::string trim(const std::string& in, const std::string& toTrim);
std::string_view trim(std::string_view in, const std::string_view& toTrim);

/**
 * Checks if a string starts with a given prefix.
 *
 * @param in The string.
 * @param prefix The prefix.
 * @returns true if the passed string starts with the prefix,
 * false otherwise.
 */
inline constexpr bool starts_with( const std::string_view& in, const std::string_view& prefix )
{
    return in.rfind( prefix, 0 ) != std::string::npos;
}

/**
 * Checks if a string ends with a given suffix.
 *
 * @param in The string.
 * @param suffix The suffix.
 * @returns true if the passed string ends with the suffix,
 * false otherwise.
 */
inline constexpr bool ends_with( const std::string_view& in, const std::string_view& suffix )
{
    return in.find( suffix, in.length() - suffix.length() ) != std::string::npos;
}

/**
 * Converts a container of one type to a container of another type using a
 * converter function or lambda.
 */
template <typename Container, typename ConverterFn>
auto transform( const Container& in, ConverterFn converter )
{
    using Target = std::decay_t<decltype(converter(std::declval<typename Container::value_type>()))>;
    std::vector<Target> result;
    result.reserve(in.size());

    for (const auto& element : in) {
        result.push_back(converter(element));
    }

    return result;
}

/**
 * Converts a vector of one type to a vector of another type using an
 * overloaded function pointer. The correct overload is selected based on
 * the vector's element type.
 */
template <typename T, typename Ret>
std::vector<Ret> transform( const std::vector<T>& in, Ret(*converter)(T) )
{
    // The non-deduced context rule is one of those C++ subtleties that bites
    // everyone at least once. The short version to remember: if T only appears
    // inside typename SomeTemplate<T>::something, the compiler won't deduce T
    // from it — it must appear "bare" somewhere in the parameter list.

    std::vector<Ret> result;
    result.reserve(in.size());

    for (const auto& element : in) {
        result.push_back(converter(element));
    }

    return result;
}

/**
 * Resource management with explicit non-destructor-based release.
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
     * Creates a Disposer with a handle argument and a cleanup
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

} // namespace smack

// ---------------------------------------------------------------------------
// Deprecated: smack::strings — use smack:: directly.
// ---------------------------------------------------------------------------

namespace smack::strings {

[[deprecated("Use smack::split instead")]]
inline std::vector<std::string> split(const std::string& in, const std::string& delimiter)
{ return smack::split(in, delimiter); }

[[deprecated("Use smack::concat instead")]]
inline std::string concat(const std::vector<std::string>& in, const std::string& delimiter)
{ return smack::concat(in, delimiter); }

[[deprecated("Use smack::trim instead")]]
inline std::string trim(const std::string& in)
{ return smack::trim(in); }

[[deprecated("Use smack::trim instead")]]
inline std::string_view trim(std::string_view in)
{ return smack::trim(in); }

[[deprecated("Use smack::trim instead")]]
inline std::string trim(const std::string& in, const std::string& toTrim)
{ return smack::trim(in, toTrim); }

[[deprecated("Use smack::trim instead")]]
inline std::string_view trim(std::string_view in, const std::string_view& toTrim)
{ return smack::trim(in, toTrim); }

[[deprecated("Use smack::starts_with instead")]]
inline constexpr bool starts_with(const std::string_view& in, const std::string_view& prefix)
{ return smack::starts_with(in, prefix); }

[[deprecated("Use smack::ends_with instead")]]
inline constexpr bool ends_with(const std::string_view& in, const std::string_view& suffix)
{ return smack::ends_with(in, suffix); }

} // namespace smack::strings

// ---------------------------------------------------------------------------
// Deprecated: smack::util — use smack:: directly.
// ---------------------------------------------------------------------------

namespace smack::util {

template <typename T, typename C>
using Disposer [[deprecated("Use smack::Disposer instead")]] = smack::Disposer<T, C>;

namespace strings {

[[deprecated("Use smack::split instead")]]
inline std::vector<std::string> split(const std::string& in, const std::string& delimiter)
{ return smack::split(in, delimiter); }

[[deprecated("Use smack::concat instead")]]
inline std::string concat(const std::vector<std::string>& in, const std::string& delimiter)
{ return smack::concat(in, delimiter); }

[[deprecated("Use smack::trim instead")]]
inline std::string trim(const std::string& in)
{ return smack::trim(in); }

[[deprecated("Use smack::trim instead")]]
inline std::string_view trim(std::string_view in)
{ return smack::trim(in); }

[[deprecated("Use smack::trim instead")]]
inline std::string trim(const std::string& in, const std::string& toTrim)
{ return smack::trim(in, toTrim); }

[[deprecated("Use smack::trim instead")]]
inline std::string_view trim(std::string_view in, const std::string_view& toTrim)
{ return smack::trim(in, toTrim); }

[[deprecated("Use smack::starts_with instead")]]
inline constexpr bool starts_with(const std::string_view& in, const std::string_view& prefix)
{ return smack::starts_with(in, prefix); }

[[deprecated("Use smack::ends_with instead")]]
inline constexpr bool ends_with(const std::string_view& in, const std::string_view& suffix)
{ return smack::ends_with(in, suffix); }

} // namespace strings

} // namespace smack::util

