/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2021 Michael Binz
 */

#include <gtest/gtest.h>

#include <string>

#include <smack_properties.hpp>
#include <smack_util.hpp>
#include "test_common.hpp"

using std::string;
using std::vector;

using smack::util::properties::PropertyMap;
using smack::util::properties::Properties;
using smack::util::properties::loadProperties;

TEST(UtilProperties, Basic) {
    PropertyMap map = loadProperties("../../test/resources/smack_properties.props");
    ASSERT_EQ( "8", map["propertyCount"] );
}

TEST(UtilProperties, BasicObject) {
    Properties map{ "../../test/resources/smack_properties.props" };
    ASSERT_EQ("8", map.get("propertyCount"));
}

TEST(UtilProperties, GetTrimmedKey) {
    Properties map{ "../../test/resources/smack_properties.props" };
    ASSERT_EQ("313", map.get("trimmed"));
}

TEST(UtilProperties, GetAs) {
    Properties map{ "../../test/resources/smack_properties.props" };
    ASSERT_EQ(8, map.getAs<int>("propertyCount"));
}

TEST(UtilProperties, GetAsFloat) {
    Properties map{ "../../test/resources/smack_properties.props" };
    ASSERT_EQ(8.0f, map.getAs<float>("propertyCount"));
}

TEST(UtilProperties, GetAsNotFound) {
    Properties map{ "../../test/resources/smack_properties.props" };
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
    Properties map{ "../../test/resources/smack_properties_continuation.props" };
    ASSERT_TRUE(map.fileFound());
    ASSERT_EQ(4, map.size());
    ASSERT_EQ("one", map.get("one"));
    ASSERT_EQ("two", map.get("two"));
    ASSERT_EQ("three", map.get("three"));
    ASSERT_EQ("eof", map.get("eof"));
}
