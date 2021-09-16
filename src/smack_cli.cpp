/* $Id: 528b24919929c65ff5bf571071f613e1925b7461 $
 *
 * Console application helper.
 *
 * Copyright (c) 2019-2021 Michael Binz
 */

#include "smack_cli.hpp"

#include <charconv>
#include <limits>
#include <sstream>
#include "smack_util.hpp"

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

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
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

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
auto convert_( std::string_view str, T& result ) {
    return std::from_chars(str.data(), str.data() + str.size(), result);
}

#if 0
#endif
template <typename T>
auto transformImpl2(
    const char* in,
    const char* type = get_typename<T>()) -> T
{
    std::string_view str{ in };

    try {
        T result{};

        auto r = convert_(str, result);

        if (r.ec == std::errc())
        {
//            std::cout << "Result: " << result << ", ptr -> " << std::quoted(r.ptr) << '\n';
            if (*r.ptr==0)
                return result;
            throw std::invalid_argument(r.ptr);
        }
        else if (r.ec == std::errc::invalid_argument)
        {
//            std::cout << "That isn't a number.\n";
            throw std::invalid_argument("Not a number.");
        }
        else if (r.ec == std::errc::result_out_of_range)
        {
//            std::cout << "This number is larger than an int.\n";
            throw std::out_of_range("ov");
        }
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
}

} // namespace anonymous

// Define the explict instantiations of the conversion functions.

template<> void transform(const char* in, char& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, signed char& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned char& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, short& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, int& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, long& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, long long& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned short& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned int& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned long& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, unsigned long long& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, float& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
}

template<> void transform(const char* in, double& out) {
    out = transformImpl2<typename std::decay<decltype(out)>::type>(in);
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
