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


template <typename T>
void transform_impl_signed(const char* in, T& out, const char* tname) {
    std::size_t pos;
    try {
        long long result = std::stoll(in, &pos, 0);
        // If all input is processed ...
        if (!in[pos]) {
            // ... and is in range ...
            out = validateRange<T>(result);
            // ... were done
            return;
        }
    }
    catch (const std::invalid_argument&) {
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

template <typename R, R (*T)(int&)>
void doOperation()
{
    int temp = 0;
    T(temp);
    std::cout << "Result is " << temp << std::endl;
}

template <typename R, R(F)(const string&, std::size_t*, int), typename T>
void transform_impl(const char* in, T& out, const char* tname) {
    std::size_t pos = 0;
    try {
        R result = F(in, &pos, 0);
        // If all input is processed ...
        if (!in[pos]) {
            // ... and is in range ...
            out = validateRangeX<T>(result);
            // ... were done
            return;
        }
    }
    catch (const std::invalid_argument&) {
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

} // namespace anonymous

template<> void transform(const char* in, char& out) {
    transform_impl<long long, std::stoll>(in, out, "char");
}

template<> void transform(const char* in, int& out) {
    transform_impl_signed(in, out, "int");
}

template<> void transform(const char* in, long& out) {
    transform_impl_signed(in, out, "long");
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
