/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Type conversions.
 *
 * Copyright © 2021 Michael Binz
 */

#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace smack::convert {

using std::size_t;
using std::string;

class conversion_failure : public std::runtime_error {
public:
    using base = std::runtime_error;
    explicit conversion_failure(const string& msg) : base(msg.c_str()) {}
    explicit conversion_failure(const char* msg) : base(msg) {}
};

/**
 * Define the transformation function.  Implementations for primitives
 * and string-like types are available in the implementation file.
 */
template <typename From, typename To>
void transform(From in, To& out) {
    static_assert(!sizeof(To),
        "No smack::convert::transform specialization found for this type. "
        "Provide: template<> void smack::convert::transform(const char*, YourType&)");
}

// Declarations of the built-in specializations defined in smack_convert.cpp.
// These prevent the static_assert above from firing for the supported types.
template<> void transform(const char* in, char& out);
template<> void transform(const char* in, signed char& out);
template<> void transform(const char* in, unsigned char& out);
template<> void transform(const char* in, short& out);
template<> void transform(const char* in, int& out);
template<> void transform(const char* in, long& out);
template<> void transform(const char* in, long long& out);
template<> void transform(const char* in, unsigned short& out);
template<> void transform(const char* in, unsigned int& out);
template<> void transform(const char* in, unsigned long& out);
template<> void transform(const char* in, unsigned long long& out);
template<> void transform(const char* in, float& out);
template<> void transform(const char* in, double& out);
template<> void transform(const char* in, bool& out);
template<> void transform(const char* in, string& out);
template<> void transform(const char* in, const char*& out);

template <typename To>
void transform(const std::string& in, To& out) {
    transform(in.c_str(), out);
}

using cstr = const char*;

template<int N>
struct Choice: Choice<N-1>
{};

template<> struct Choice<0>
{};

template <typename T, std::enable_if_t<std::is_reference<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<6>) {
    using i1 = typename std::remove_reference<T>::type;
    using i2 = typename std::remove_const<i1>::type;

    return get_typename_<i2>(Choice<4>{});
}

template <typename T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<5>) {
    using i1 = typename std::remove_pointer<T>::type;
    using i2 = typename std::remove_const<i1>::type;
    using i3 = typename std::add_pointer<i2>::type;

    return get_typename_<i3>(Choice<4>{});
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<4>) {
    const auto digits = std::numeric_limits<T>::digits;

    if ( digits == 1 )
        return "bool";

    if ( sizeof( T ) == 1 ) {
            return std::numeric_limits<T>::min() == 0 ?
                "ubyte" : "byte";
    }
    if ( sizeof( T ) == 2 ) {
            return std::numeric_limits<T>::min() == 0 ?
                "ushort" : "short";
    }
    if ( sizeof( T ) == 4 ) {
            return std::numeric_limits<T>::min() == 0 ?
                "uint" : "int";
    }

    return std::numeric_limits<T>::min() == 0 ?
        "ulong" : "long";
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<3>) {
    const auto digits = std::numeric_limits<T>::digits;

    if ( digits <= std::numeric_limits<float>::digits )
        return "float";
    if ( digits <= std::numeric_limits<double>::digits )
        return "double";

    return "ldouble";
}

template<typename T,
std::enable_if_t<std::is_same<T, string>::value, bool> = true>
constexpr cstr get_typename_(Choice<2>) {
    return "string";
}

template<typename T,
std::enable_if_t<std::is_same<T, char*>::value, bool> = true>
constexpr cstr get_typename_(Choice<1>) {
    return "string";
}

template<typename T>
const char* get_typename_() {
    // Fallback: returns the compiler-mangled type name.
    // For a readable name, provide an explicit specialisation of get_typename<T>().
    return typeid(T).name();
}

template<typename T>
const char* get_typename_(Choice<0>) {
    return get_typename_<T>();
}

/**
 * @return a string representation for the supported types.
 * To add a name for a custom type, provide an explicit specialization of this function.
 */
template<typename T>
const char* get_typename( T ) {
    return get_typename_<T>(Choice<10>{});
}

/**
 * @return a string representation for the supported types.
 */
template<typename T>
const char* get_typename() {
    if constexpr (std::is_default_constructible_v<T>)
        return get_typename(T{});
    else
        return get_typename_<T>(Choice<10>{});
}

} // namespace smack::convert
