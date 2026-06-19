/*
 * Smack C++
 *
 * Copyright (c) 2019-2021 Michael Binz
 */

#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

namespace smack::examples {

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

} // namespace smack::test::test
