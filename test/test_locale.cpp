/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2026 Michael Binz
 */
#include <gtest/gtest.h>

#include <filesystem>
#include <set>
#include <algorithm>
#include <string>
#include <vector>

#include <smack_util.hpp>
#include <smack_system.h>
#include "test_common.hpp"

using Locale = smack::Locale;

TEST(Locale, constructor)
{
    ASSERT_THROW(
        Locale l("de_"),
        std::exception
    );
    ASSERT_THROW(
        Locale l("de", "BY_"),
        std::exception
    );
    {
        Locale l("de");

        ASSERT_EQ("de", l.toString());
        ASSERT_EQ("de", l.getLanguage());
        ASSERT_EQ("", l.getCountry());
    }
    {
        Locale l("de", "DE");

        ASSERT_EQ("de_DE", l.toString());
        ASSERT_EQ("de", l.getLanguage());
        ASSERT_EQ("DE", l.getCountry());
    }
}

TEST(Locale, toString)
{
    {
        Locale l;

        ASSERT_EQ(
            "",
            l.toString());
    }
    {
        Locale l("country");
        ASSERT_EQ(
            "country",
            l.toString());
    }
    {
        Locale l("country", "language");
        ASSERT_EQ(
            "country_language",
            l.toString());
    }
}

TEST(Locale, current_locale_setGet)
{
    auto originalLocale = Locale::getCurrent();

    Locale::setCurrent(Locale{});

    ASSERT_TRUE(
        Locale::getCurrent().toString().empty());

    Locale::setCurrent(
        Locale{ "de", "DE" });
    ASSERT_EQ(
        "de_DE",
        Locale::getCurrent().toString());

    Locale::setCurrent(
        Locale{ "en", "US" });
    ASSERT_EQ(
        "en_US",
        Locale::getCurrent().toString());

    Locale::setCurrent(
        Locale{ "it", "US" });
    ASSERT_EQ(
        "it_US",
        Locale::getCurrent().toString());

    Locale::setCurrent(
        originalLocale );
}

TEST(Locale, LocaleEq) {

    Locale enUs1{ "en", "US" };
    Locale enUs2{ "en", "US" };
    ASSERT_EQ(enUs1, enUs2);
    Locale enGb{ "en", "GB" };
    ASSERT_FALSE(enUs1 == enGb);
}

TEST(Locale, GetParentHasCountry) {
    Locale enGb{ "en", "GB" };
    Locale parent = enGb.getParent();
    ASSERT_EQ("en", parent.getLanguage());
    ASSERT_EQ("", parent.getCountry());
}

TEST(Locale, GetParentNoCountry) {
    Locale en{ "en" };
    Locale parent = en.getParent();
    ASSERT_TRUE(parent.isEmpty());
}

TEST(Locale, GetParentEmpty) {
    Locale empty{};
    Locale parent = empty.getParent();
    ASSERT_TRUE(parent.isEmpty());
}

TEST(Locale, makeLocaleFromName_EmptyStringReturnsEmptyLocale)
{
    auto locale = Locale::makeLocaleFromName("");

    ASSERT_TRUE(locale.isEmpty());
}

TEST(Locale, makeLocaleFromName_LanguageOnly)
{
    auto locale = Locale::makeLocaleFromName("de");

    ASSERT_EQ("de", locale.getLanguage());
    ASSERT_EQ("",   locale.getCountry());
    ASSERT_EQ("de", locale.toString());
}

TEST(Locale, makeLocaleFromName_LanguageAndCountry)
{
    auto locale = Locale::makeLocaleFromName("en_GB");

    ASSERT_EQ("en",    locale.getLanguage());
    ASSERT_EQ("GB",    locale.getCountry());
    ASSERT_EQ("en_GB", locale.toString());
}

TEST(Locale, makeLocaleFromName_ThrowsOnTooManyParts)
{
    ASSERT_THROW(
        Locale::makeLocaleFromName("en_GB_extra"),
        std::invalid_argument);
}

TEST(Locale, CurrentLocaleEqualsSystemLocale) {
    ASSERT_EQ(
        Locale::getCurrent(),
        smack::system::getLocale() );
}
