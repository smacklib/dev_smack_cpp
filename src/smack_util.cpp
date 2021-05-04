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
using std::vector;

string strings::trim(const string& in)
{
    auto pred = [](int c) {
        // Note that isspace may be inlined, thus the wrapping into
        // a lambda.
        return std::isspace( c );
    };

    return strings::trim_(in, pred);
}

string strings::trim(const string& in, const string& toTrim)
{
    auto pred = [&toTrim](int c) {
        return string::npos != toTrim.find_first_of(c);
    };

    return strings::trim_(in, pred);
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

    string result;

    for (size_t i = 0; i < in.size(); ++i)
    {
        result += in[i];
        // Not the last index.
        if ( i < in.size()-1 )
            result += delimiter;
    }

    return result;
}

} // namepace util
} // namespace smack
