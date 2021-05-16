/*
 * Smack C++
 *
 * Copyright (c) 2019-2021 Michael Binz
 */

#pragma once

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "smack_util.hpp"

namespace smack {
namespace test {
namespace common {

inline void pr() {
}

template <typename A, typename ... D>
void pr(A& car, D ... cdr) {
    std::cout << car;
    if (sizeof ... (cdr)) {
        std::cout << ", ";
        pr(cdr ...);
    }
}

template <typename ... TT>
int f(const char* func, TT ... args) {
    std::cout << func << "( ";
    pr(args ...);
    std::cout << " )" << std::endl;

    return EXIT_SUCCESS;
}

/**
 * Temporarily redirect a stream.
 */
class redir 
{
private:
    std::ostream& ss_;
    std::streambuf* original_;
    std::stringstream buffer_;

public:
    /**
     * Create an instance.
     * @param ss The stream to redirect.
     */
    redir(std::ostream& ss) : ss_(ss)
    {
        original_ = ss_.rdbuf(
            buffer_.rdbuf());
    }
    ~redir()
    {
        ss_.rdbuf(original_);
    }

    /**
     * Get the content from the redirected stream. 
     */
    std::string str()
    {
        return buffer_.str();
    }

    /**
     * Get the content from the redirected stream.
     */
    std::vector<std::string> strs()
    {
        return smack::util::strings::split(
            buffer_.str(),
            "\n");
    }
};

} // namespace common
} // namespace test
} // namespace smack
