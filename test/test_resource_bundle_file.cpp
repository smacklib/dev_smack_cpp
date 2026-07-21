/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2019-2025 Michael Binz
 */
#include <gtest/gtest.h>

#include <filesystem>

#include <smack_resource_bundle.h>
#include <smack_util.hpp>
#include "test_common.hpp"

using ResourceBundleFile = smack::localisation::ResourceBundleFile;

namespace {
    const std::filesystem::path PROJ_RESOURCE_DIR =
        smack::test::TEST_DIR / "resources/resourceBundle";
}

TEST(ResourceBundleFile, resourceDirExists)
{
    ASSERT_TRUE(
        std::filesystem::exists(PROJ_RESOURCE_DIR));
}

TEST(ResourceBundleFile, toString)
{
    ResourceBundleFile rb{ "smack", PROJ_RESOURCE_DIR };

    ASSERT_TRUE(
        smack::starts_with(
            rb.toString(),
            "ResourceBundleFile{smack@" + PROJ_RESOURCE_DIR.string() + "}"));
}
