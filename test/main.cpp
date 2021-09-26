/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2021 Michael Binz
 */

#include <string>
#include <gtest/gtest.h>
#include "test_common.hpp"

std::string smack::test::common::g_gtestExecutableName;

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);

    if ( argc > 0 )
        smack::test::common::g_gtestExecutableName = argv[0];

    return RUN_ALL_TESTS();
}
