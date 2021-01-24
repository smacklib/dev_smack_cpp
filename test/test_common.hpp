/*
 * Copyright � 2019 Daimler TSS
 */

#pragma once

#include <cstdlib>
#include <iostream>

namespace tss {
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

} // namespace common
} // namespace test
} // namespace tss