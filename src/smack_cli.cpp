/* $Id: 528b24919929c65ff5bf571071f613e1925b7461 $
 *
 * Console application helper.
 *
 * Copyright (c) 2019-2021 Michael Binz
 */

#include "smack_cli.hpp"

namespace smack::cli {

template<> void transform(const char* in, int& out) {
    std::size_t pos;
    out = std::stoi(in, &pos, 0);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
}

template<> void transform(const char* in, long& out) {
    std::size_t pos;
    out = std::stol(in, &pos, 0);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
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
