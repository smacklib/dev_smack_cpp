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

    throw std::invalid_argument(msg.str());
}

template <typename T>
T validateRange(long long val) {
    if (val < std::numeric_limits<T>::min()) {
        throw std::out_of_range("");
    }
    if (val > std::numeric_limits<T>::max()) {
        throw std::out_of_range("");
    }

    return static_cast<T>(val);
}
/**
 * Validate if a value from a wider type is in the limits of a narrower type.
 * F is wider, T is the narrower type.
 */
template <typename T, typename F>
T validateRangeX(F val) {
    if (val < std::numeric_limits<T>::min()) {
        throw std::out_of_range("");
    }
    if (val > std::numeric_limits<T>::max()) {
        throw std::out_of_range("");
    }

    return static_cast<T>(val);
}

/**
 * Conversion function specialisation.
 */
template <auto F>
struct ConvFu {};

/**
 * Specialisation for free functions. 
 */
template <typename R, typename... Args, auto (F)(Args...)->R>
struct ConvFu<F> 
{
    template <typename T, typename Fu>
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
                out = validateRangeX<T>(result);
                // ... we're done
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
                static_cast<long>(std::numeric_limits<T>::min()) <<
                ".." <<
                static_cast<long>(std::numeric_limits<T>::max()) <<
                "].";

            throw std::invalid_argument(msg.str());
        }

        throwConversionFailure(in, tname);
    }
};

template <auto F, typename T>
auto transformImpl( 
    const char* in, 
    T& out, 
    const char* tname ) {
    return ConvFu<F>::make(
        F,
        in,
        out,
        tname
    );
}

// Select the string overloads of the respective standard functions.
constexpr auto cvSigned = 
    static_cast<long long(*)(const string&, size_t*, int)>(std::stoll);
constexpr auto cvUnsigned = 
    static_cast<unsigned long long(*)(const string&, size_t*, int)>(std::stoull);
constexpr auto stold_ = 
    static_cast<long double(*)(const string&, size_t*)>(std::stold);

} // namespace anonymous

// Define the explict instantiations of the conversion functions.

template<> void transform(const char* in, char& out) {
    transformImpl<cvSigned>(in, out, "char");
}

template<> void transform(const char* in, int& out) {
    transformImpl<cvSigned>(in, out, "int");
}

template<> void transform(const char* in, long& out) {
    transformImpl<cvSigned>(in, out, "long");
}

template<> void transform(const char* in, unsigned char& out) {
    transformImpl<cvUnsigned>(in, out, "char");
}

template<> void transform(const char* in, unsigned int& out) {
    transformImpl<cvUnsigned>(in, out, "int");
}

template<> void transform(const char* in, unsigned long& out) {
    transformImpl<cvUnsigned>(in, out, "long");
}

template<> void transform(const char* in, float& out) {
    std::size_t pos;
    out = std::stof(in, &pos);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
}

template<> void transform(const char* in, double& out) {
    std::size_t pos;
    out = std::stod(in, &pos);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
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

    throw std::invalid_argument(in);
}

template<> void transform(const char* in, std::string& out) {
    out = in;
}

template<> void transform(const char* in, const char*& out) {
    out = in;
}

} // namespace smack::util
