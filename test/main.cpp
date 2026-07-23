/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2021-2026 Michael Binz
 */

#include <filesystem>
#include <string>
#include <gtest/gtest.h>

#include "smack_system.h"
#include "test_common.hpp"

std::string smack::test::g_gtestExecutableName;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    if ( argc > 0 )
        smack::test::g_gtestExecutableName = argv[0];

    std::filesystem::current_path(
        smack::system::executablePath().parent_path());

    return RUN_ALL_TESTS();
}
