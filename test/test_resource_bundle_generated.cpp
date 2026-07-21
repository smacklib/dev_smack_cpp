/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests for the generated ResourceBundle class rb_smack.
 *
 * Copyright © 2026 Michael Binz
 */

#include <gtest/gtest.h>

#include <set>
#include <string>

#include <rb_smack.h>

using Locale = smack::Locale;

class RbSmackTest : public ::testing::Test {
protected:
    void SetUp() override {
        savedLocale_ = Locale::setCurrent(Locale{});
    }

    void TearDown() override {
        Locale::setCurrent(savedLocale_);
    }

    Locale savedLocale_;
    smack::localisation::ResourceBundleGenerated& rb = smack::rb_smack;
};

TEST_F(RbSmackTest, toString_ContainsKnownLocaleKeys)
{
    const auto result = rb.toString();

    ASSERT_TRUE( smack::starts_with(result, "ResourceBundle:smack\n{ cn, de, en, en_GB, es, fr }\nCreated: ") );
}
