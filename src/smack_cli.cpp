/* $Id: 528b24919929c65ff5bf571071f613e1925b7461 $
 *
 * Console application helper.
 *
 * Copyright (c) 2019-2021 Michael Binz
 */

#include "smack_cli.hpp"

#include <limits>
#include <sstream>

namespace smack::cli {

namespace {

using std::to_string;

/**
 * Common error handling.
 */
void throwConversionFailure(const char* what, const char* type) {
    std::ostringstream msg;

    msg <<
        "Cannot convert '" <<
        what <<
        "' to " <<
        type <<
        ".";

    throw conversion_failure(msg.str());
}

/**
 * Validate if a value from a wider type is in the limits of a narrower type.
 * F is wider, T is the narrower type.
 */
template <typename T, typename F>
T validateRange(F val) {
    T downcast = 
        static_cast<T>(val);
    F upcast = 
        downcast;

    if ( upcast != val )   
        throw std::out_of_range("");

    return downcast;
}

/**
 * Conversion function specialisation.
 */
template <auto F>
struct ConvFu {};

template <typename R, typename... Args, auto (F)(Args...)->R>
struct ConvFu<F> 
{
    template <typename T, typename Fu, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    static void make(
        Fu function,
        const char* in,
        T& out,
        const char* tname)
    {
        std::size_t pos = 0;
        try {
            R result = function(in, &pos, 0);
            // If all input is processed ...
            if (!in[pos]) {
                // ... and is in range ...
                out = validateRange<T>(result);
                // ... we're done
                return;
            }
        }
        catch (const std::invalid_argument&) {
            // Ignore this exception.  A corresponding exception
            // with a better message is thrown below. 
        }
        catch (const std::out_of_range&) {
            std::ostringstream msg;

            msg <<
                "Value " <<
                in <<
                " must be in range [" <<
                to_string(std::numeric_limits<T>::min()) <<
                ".." <<
                to_string(std::numeric_limits<T>::max()) <<
                "].";

            throw conversion_failure(msg.str());
        }

        throwConversionFailure(in, tname);
    }

    template <typename T, typename Fu, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    static void make(
        Fu function,
        const char* in,
        T& out,
        const char* tname)
    {
        std::size_t pos = 0;
        try {
            R result = function(in, &pos);
            // If all input is processed ...
            if (!in[pos]) {
                // ... we're done.
                out = result;
                return;
            }
        }
        catch (const std::invalid_argument&) {
            // Ignore this expection.  A corresponding exception
            // with a better message is thrown below. 
        }
        catch (const std::out_of_range&) {
            std::ostringstream msg;

            msg <<
                "Value " <<
                in <<
                " must be in range [" <<
                to_string(std::numeric_limits<T>::min()) <<
                ".." <<
                to_string(std::numeric_limits<T>::max()) <<
                "].";

            throw conversion_failure(msg.str());
        }

        throwConversionFailure(in, tname);
    }
};

template <auto F, typename T>
auto transformImpl( 
    const char* in, 
    T& out, 
    const char* tname = get_typename<T>()) {
    return ConvFu<F>::make(
        F,
        in,
        out,
        tname
    );
}

// Select the string overloads of the respective standard functions.
constexpr auto cvStoi = 
    static_cast<int(*)(const string&, size_t*, int)>(std::stoi);
constexpr auto cvStol = 
    static_cast<long(*)(const string&, size_t*, int)>(std::stol);
constexpr auto cvStoll = 
    static_cast<long long(*)(const string&, size_t*, int)>(std::stoll);
constexpr auto cvUnsigned = 
    static_cast<unsigned long long(*)(const string&, size_t*, int)>(std::stoull);
constexpr auto cvFloat = 
    static_cast<float(*)(const string&, size_t*)>(std::stof);
constexpr auto cvDouble = 
    static_cast<double(*)(const string&, size_t*)>(std::stod);

} // namespace anonymous

// Define the explict instantiations of the conversion functions.

template<> void transform(const char* in, char& out) {
    transformImpl<cvUnsigned>(in, out);
}

template<> void transform(const char* in, signed char& out) {
    transformImpl<cvStoi>(in, out);
}

template<> void transform(const char* in, unsigned char& out) {
    transformImpl<cvUnsigned>(in, out);
}

template<> void transform(const char* in, short& out) {
    transformImpl<cvStoi>(in, out);
}

template<> void transform(const char* in, int& out) {
    transformImpl<cvStoi>(in, out);
}

template<> void transform(const char* in, long& out) {
    transformImpl<cvStol>(in, out);
}

template<> void transform(const char* in, long long& out) {
    transformImpl<cvStoll>(in, out);
}

template<> void transform(const char* in, unsigned short& out) {
    transformImpl<cvUnsigned>(in, out);
}

template<> void transform(const char* in, unsigned int& out) {
    transformImpl<cvUnsigned>(in, out);
}

template<> void transform(const char* in, unsigned long& out) {
    transformImpl<cvUnsigned>(in, out);
}

template<> void transform(const char* in, unsigned long long& out) {
    transformImpl<cvUnsigned>(in, out);
}

template<> void transform(const char* in, float& out) {
    transformImpl<cvFloat>(in, out);
}

template<> void transform(const char* in, double& out) {
    transformImpl<cvDouble>(in, out);
}

template<> void transform(const char* in, bool& out) {
    std::string parameter = in;

    if (parameter == "true") {
        out = true;
        return;
    }
    if (parameter == "false") {
        out = false;
        return;
    }

    throwConversionFailure(in, get_typename<bool>());
}

template<> void transform(const char* in, std::string& out) {
    out = in;
}

template<> void transform(const char* in, const char*& out) {
    out = in;
}

} // namespace smack::cli
