/*
 * Smack C++
 *
 * Copyright © 2019-2021 Michael Binz
 */

#include <cctype>
#include <iostream>

#include "smack_util.hpp"

namespace smack {
namespace util {

using std::string;
using std::string_view;
using std::vector;

namespace {
template <typename T>
void trimInplace_(string_view& in, const T& pred)
{
    while (!in.empty() && pred(in[0]))
        in = in.substr(1);
    while (!in.empty() && pred(in[in.size() - 1]))
        in = in.substr(0, in.size() - 1);
}
}

string strings::trim(const string& in)
{
    string_view inv{ in };
    inv = strings::trim(inv);
    return string{inv};
}

string_view strings::trim(string_view in)
{
    auto pred = [](int c) {
        // Note that isspace may be inlined, thus the wrapping into
        // a lambda.
        return std::isspace(c);
    };

    trimInplace_(in, pred);
    return in;
}

string strings::trim(const string& in, const string& toTrim)
{
    string_view inv{ in };
    inv = strings::trim(inv, string_view{toTrim});
    return string{ inv };
}

string_view strings::trim(string_view in, const string_view& toTrim)
{
    auto pred = [&toTrim](int c) {
        return string::npos != toTrim.find_first_of(c);
    };

    trimInplace_(in, pred);
    return in;
}

vector<string> strings::split(const string& in, const string& delimiter)
{
    std::vector<std::string> result;

    // An empty string results in an empty token vector.
    if (in.empty())
        return result;

    // An empty delimiter results in a single token.
    if (delimiter.empty())
    {
        result.push_back(in);
        return result;
    }

    const auto delimiterLength =
        delimiter.length();

    for (size_t ppos = 0, pos = 0; ; ppos = pos + delimiterLength )
    {
        pos = in.find(delimiter, ppos);

        if (pos == string::npos)
        {
            result.push_back(in.substr(ppos));
            return result;
        }

        result.push_back(
            in.substr(ppos, pos-ppos));
    }
}

string strings::concat(const vector<string>& in, const string& delimiter)
{
    if (in.empty())
        return string{};

    string result = in[0];

    for (size_t i = 1; i < in.size(); ++i)
        result.append( delimiter ).append( in[i] );

    return result;
}

} // namepace util
} // namespace smack
