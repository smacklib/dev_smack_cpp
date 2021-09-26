/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Type conversions.
 *
 * Copyright © 2021 Michael Binz
 */

#include "smack_convert.hpp"

#include <charconv>
#include <limits>
#include <sstream>
#include "smack_util.hpp"

namespace smack::convert {

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

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
auto convert_(std::string_view str, T& result)
{
    int radix = 10;

    if (smack::util::strings::starts_with(str, "0x")) {
        radix = 16;
        str = str.substr(2);
    }
    else if (smack::util::strings::starts_with(str, "0b")) {
        radix = 2;
        str = str.substr(2);
    }

    return std::from_chars(str.data(), str.data() + str.size(), result, radix);
}

template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
auto convert_( std::string_view str, T& result ) {
    return std::from_chars(str.data(), str.data() + str.size(), result);
}

template <typename T>
auto transform_(
    std::string_view in,
    const char* type = get_typename<T>()) -> T
{
    std::string_view str{ in };

    T result{};

    try {
        auto r = convert_(str, result);

        if (r.ec == std::errc() && *r.ptr)
            throw std::invalid_argument(r.ptr);
        else if (r.ec == std::errc::invalid_argument)
            throw std::invalid_argument("Not a number.");
        else if (r.ec == std::errc::result_out_of_range)
            throw std::out_of_range("ov");
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
    catch (const std::invalid_argument&) {
        std::ostringstream msg;

        msg <<
            "Cannot convert '" <<
            in <<
            "' to " <<
            type <<
            ".";

        throw conversion_failure(msg.str());
    }

    return result;
}

} // namespace anonymous

// Define the explict instantiations of the conversion functions.

template<> void transform(const char* in, char& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, signed char& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned char& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, short& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, int& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, long& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, long long& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned short& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned int& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned long& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned long long& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, float& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, double& out) {
    out = transform_<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, bool& out) {
    std::string_view parameter{ in };

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
