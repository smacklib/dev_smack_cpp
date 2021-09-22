/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Type conversions.
 *
 * Copyright © 2021 Michael Binz
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace smack::convert {

using std::cerr;
using std::cout;
using std::endl;
using std::initializer_list;
using std::size_t;
using std::string;
using std::vector;

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
void transform(From in, To& out);

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
constexpr cstr get_typename_() {
    return "unknown"; 
}

template<typename T>
constexpr cstr get_typename_(Choice<0>) {
    return get_typename_<T>(); 
}

/**
 * @return a string representation for the supported types.
 */
template<typename T>
constexpr cstr get_typename() { 
    return get_typename_<T>(Choice<10>{}); 
}

/**
 * @return a string representation for the supported types.
 */
template<typename T>
constexpr cstr get_typename( T type ) { 
    return get_typename<decltype(type)>(); 
}

} // namespace smack::convert
