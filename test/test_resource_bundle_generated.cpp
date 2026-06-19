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

// ---------------------------------------------------------------------------
// tl() — translation lookups
// ---------------------------------------------------------------------------

TEST_F(RbSmackTest, tl_EnglishLocale)
{
    Locale::setCurrent(Locale{"en"});

    EXPECT_EQ("Trashcan", rb.tl("smack.trash"));
    EXPECT_EQ("Yes",      rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_GermanLocale)
{
    Locale::setCurrent(Locale{"de"});

    EXPECT_EQ("Papierkorb", rb.tl("smack.trash"));
    EXPECT_EQ("Ja",         rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_ChineseLocale)
{
    Locale::setCurrent(Locale{"cn"});

    EXPECT_EQ("\xe5\x9e\x83\xe5\x9c\xbe\xe6\xa1\xb6", rb.tl("smack.trash"));
    EXPECT_EQ("\xe6\x98\xaf",                           rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_FrenchLocale)
{
    Locale::setCurrent(Locale{"fr"});

    EXPECT_EQ("Oui", rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_SpanishLocale)
{
    Locale::setCurrent(Locale{"es"});

    EXPECT_EQ("S\xc3\xad", rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_BritishEnglishUsesExactMatch)
{
    Locale::setCurrent(Locale{"en", "GB"});

    EXPECT_EQ("Wastebasket", rb.tl("smack.trash"));
}

TEST_F(RbSmackTest, tl_BritishEnglishFallsBackToEnglishForMissingKey)
{
    Locale::setCurrent(Locale{"en", "GB"});

    // "smack.yes" is not in en_GB, so it falls back to "en".
    EXPECT_EQ("Yes", rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_DefaultLocaleReturnsDefaultBundle)
{
    // Empty locale — falls back to the default (root) bundle.
    Locale::setCurrent(Locale{});

    EXPECT_EQ("_trash", rb.tl("smack.trash"));
    EXPECT_EQ("_yes",   rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_UnknownLocale_FallsBackToDefaultBundle)
{
    Locale::setCurrent(Locale{"ja"});

    EXPECT_EQ("_trash", rb.tl("smack.trash"));
    EXPECT_EQ("_yes",   rb.tl("smack.yes"));
}

TEST_F(RbSmackTest, tl_UnknownKeyReturnsKey)
{
    Locale::setCurrent(Locale{"en"});

    const std::string unknownKey = "smack.nonexistent";
    EXPECT_EQ(unknownKey, rb.tl(unknownKey));
}

// ---------------------------------------------------------------------------
// hasDefinitions()
// ---------------------------------------------------------------------------

TEST_F(RbSmackTest, hasDefinitions_KnownLocalesReturnTrue)
{
    EXPECT_TRUE(rb.hasDefinitions(Locale{"en"}));
    EXPECT_TRUE(rb.hasDefinitions(Locale{"de"}));
    EXPECT_TRUE(rb.hasDefinitions(Locale{"cn"}));
    EXPECT_TRUE(rb.hasDefinitions(Locale{"fr"}));
    EXPECT_TRUE(rb.hasDefinitions(Locale{"es"}));
    EXPECT_TRUE(rb.hasDefinitions(Locale{"en", "GB"}));
}

TEST_F(RbSmackTest, hasDefinitions_DefaultLocaleReturnsTrue)
{
    // The root (empty) locale is present in the bundle.
    EXPECT_TRUE(rb.hasDefinitions(Locale{}));
}

TEST_F(RbSmackTest, hasDefinitions_UnknownLocaleReturnsFalse)
{
    EXPECT_FALSE(rb.hasDefinitions(Locale{"ja"}));
    EXPECT_FALSE(rb.hasDefinitions(Locale{"pl"}));
    EXPECT_FALSE(rb.hasDefinitions(Locale{"en", "US"}));
}

// ---------------------------------------------------------------------------
// listLocales()
// ---------------------------------------------------------------------------

TEST_F(RbSmackTest, listLocales_ReturnsAllNonRootLocales)
{
    const std::set<Locale> expected = {
        Locale{"cn"},
        Locale{"de"},
        Locale{"en"},
        Locale{"en", "GB"},
        Locale{"es"},
        Locale{"fr"},
    };

    EXPECT_EQ(expected, rb.listLocales());
}

TEST_F(RbSmackTest, listLocales_DoesNotContainRootLocale)
{
    const auto locales = rb.listLocales();

    EXPECT_EQ(locales.end(), locales.find(Locale{}));
}

// ---------------------------------------------------------------------------
// toString()
// ---------------------------------------------------------------------------

TEST_F(RbSmackTest, toString_ContainsBundleName)
{
    const auto result = rb.toString();

    EXPECT_NE(std::string::npos, result.find("ResourceBundle:smack"));
}

TEST_F(RbSmackTest, toString_ContainsKnownLocaleKeys)
{
    const auto result = rb.toString();

    ASSERT_TRUE( smack::starts_with(result, "ResourceBundle:smack\n{ cn, de, en, en_GB, es, fr }\nCreated: ") );
}

TEST_F(RbSmackTest, toString_ContainsKnownLocales)
{
    const auto result = rb.toString();

    EXPECT_NE(std::string::npos, result.find("en"));
    EXPECT_NE(std::string::npos, result.find("de"));
}

TEST_F(RbSmackTest, toString_getName)
{
    const auto result = rb.getName();

    ASSERT_EQ("smack", result);
}
