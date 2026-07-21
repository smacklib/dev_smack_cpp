/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2021-2026 Michael Binz
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include <smack_properties.hpp>
#include <smack_util.hpp>
#include "test_common.hpp"

using std::string;
using std::vector;

using smack::util::properties::PropertyMap;
using smack::util::properties::Properties;
using smack::util::properties::loadProperties;

namespace {
    const std::string PROPS_FILE =
        (std::filesystem::path{SMACK_TEST_RESOURCES_DIR} / "smack_properties.props").string();
    const std::string CONT_PROPS_FILE =
        (std::filesystem::path{SMACK_TEST_RESOURCES_DIR} / "smack_properties_continuation.props").string();
}

TEST(UtilProperties, Basic) {
    PropertyMap map = loadProperties(PROPS_FILE);
    ASSERT_EQ( "8", map["propertyCount"] );
}

TEST(UtilProperties, BasicObject) {
    Properties map{ PROPS_FILE };
    ASSERT_EQ("8", map.get("propertyCount"));
}

TEST(UtilProperties, GetTrimmedKey) {
    Properties map{ PROPS_FILE };
    ASSERT_EQ("313", map.get("trimmed"));
}

TEST(UtilProperties, GetAs) {
    Properties map{ PROPS_FILE };
    ASSERT_EQ(8, map.getAs<int>("propertyCount"));
}

TEST(UtilProperties, GetAsFloat) {
    Properties map{ PROPS_FILE };
    ASSERT_EQ(8.0f, map.getAs<float>("propertyCount"));
}

TEST(UtilProperties, GetAsNotFound) {
    Properties map{ PROPS_FILE };
    ASSERT_TRUE(map.fileFound());
    ASSERT_FALSE(map.hasEntry("nonExisting"));
    ASSERT_EQ(313, map.getAs<int>("nonExisting", 313));
}

TEST(UtilProperties, ObjFileNotFound) {
    Properties map{ "honk/a/tonk" };
    ASSERT_FALSE(map.fileFound());
    ASSERT_FALSE(map.hasEntry("nonExisting"));
    ASSERT_EQ(313, map.getAs<int>("nonExisting", 313));
}

TEST(UtilProperties, ContinuationLines) {
    Properties map{ CONT_PROPS_FILE };
    ASSERT_TRUE(map.fileFound());
    ASSERT_EQ(4, map.size());
    ASSERT_EQ("one", map.get("one"));
    ASSERT_EQ("two", map.get("two"));
    ASSERT_EQ("three", map.get("three"));
    ASSERT_EQ("eof", map.get("eof"));
}
