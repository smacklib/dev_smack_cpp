/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2019-2025 Michael Binz
 */
#include <gtest/gtest.h> // googletest header file

#include <filesystem>
#include <set>
#include <algorithm>
#include <string>
#include <vector>

// Disables warnings when windows.h is included.
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <Windows.h>
#include <WinNls.h>

#include <smack_resource_bundle.h>
#include <smack_properties.hpp>
#include <smack_util.hpp>
#include "test_common.hpp"

using Locale = smack::localisation::Locale;
using PropertyMap = smack::util::properties::PropertyMap;
using ResourceBundle = smack::localisation::ResourceBundle;
using std::string;
using std::vector;
using namespace std::string_literals;
namespace strings = smack::util::strings;
namespace properties = smack::util::properties;

namespace {
    const std::string PROJ_RESOURCE_DIR =
        "../../test/resources/resourceBundle/good";
}

TEST(CommonResourceBundle, resourceDirExists)
{
    ASSERT_TRUE(
        std::filesystem::exists(PROJ_RESOURCE_DIR));
}

TEST(ResourceBundleLocale, constructor)
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

TEST(CommonResourceBundle, toString)
{
    ResourceBundle rb{ "smack", PROJ_RESOURCE_DIR };

    ASSERT_EQ("ResourceBundle{smack@../../test/resources/resourceBundle/good}", rb.toString());
}

TEST(CommonResourceBundle, listLocales)
{
    ResourceBundle rb{ "smack", PROJ_RESOURCE_DIR };

    auto locales = rb.listLocales();

    ASSERT_EQ(7U, locales.size());

    std::sort( locales.begin(), locales.end() );

    size_t pos{};

    // Alphabetically sorted.
    ASSERT_EQ("", locales.at(pos++));
    ASSERT_EQ("cn", locales.at(pos++));
    ASSERT_EQ("de", locales.at(pos++));
    ASSERT_EQ("en", locales.at(pos++));
    ASSERT_EQ("en_GB", locales.at(pos++));
    ASSERT_EQ("es", locales.at(pos++));
    ASSERT_EQ("fr", locales.at(pos++));
}


TEST(CommonResourceBundle, translate_enUs)
{
    Locale en_US{ "en", "US" };
    ResourceBundle rb{ "smack", PROJ_RESOURCE_DIR };

    // We have *no* US-language.
    ASSERT_FALSE(rb.hasDefinitions(en_US));
    // Thus we resolve to en.
    ASSERT_EQ("Yes"s, rb.translate(en_US, "smack.yes"));
}


TEST(CommonResourceBundle, translate_loads_of_stuff)
{
    const string LNG = "ISO-639-1"s;
    const string CNT = "ISO-3166-2"s;

    ResourceBundle rb{ "smack", PROJ_RESOURCE_DIR };

    {
        Locale cn{ "cn" };
        ASSERT_EQ(
            "cn"s,
            rb.translate(cn, LNG));

        ASSERT_EQ(
            u8"是"s,
            rb.translate(cn, "smack.yes"));
        ASSERT_EQ(
            "垃圾桶"s,
            rb.translate(cn, "smack.trash"));
    }

    {
        Locale en{ "en" };

        ASSERT_EQ(
            "en"s,
            rb.translate(en, LNG));
        ASSERT_EQ(
            "n/a"s,
            rb.translate(en, CNT));

        ASSERT_EQ(
            "Trashcan"s,
            rb.translate( en, "smack.trash"));
    }

    {
        // Fallback to en.
        Locale enUs{ "en", "US" };

        ASSERT_EQ(
            "Trashcan"s,
            rb.translate( enUs, "smack.trash") );
    }

    {
        // en_GB exists.
        Locale enGb{ "en", "GB" };
        ASSERT_EQ(
            "Wastebasket"s,
            rb.translate( enGb, "smack.trash"));
    }

    {
        // de exists. Trivial.
        Locale de{ "de" };
        ASSERT_EQ(
            "Papierkorb"s,
            rb.translate( de, "smack.trash") );
    }

    {
        Locale zh{ "zh" };
        ASSERT_EQ(
            "_trash"s,
            rb.translate( zh, "smack.trash") );
    }

    {
        // Locale does not exist.
        Locale it{ "it" };
        ASSERT_EQ(
            "default.test.trash"s,
            rb.translate( it, "test.trash" ) );
    }

    {
        // Resolve with empty locale.  Results in fallback.
        ASSERT_EQ(
            "_trash"s,
            rb.translate( Locale{}, "smack.trash") );
    }

    {
        //  Locale exists, key *not* defined.
        ASSERT_EQ(
            "default.test.undefined"s,
            rb.translate(Locale{ "zh" }, "test.undefined"));
    }

    {
        // Locale and key unknown.
        ASSERT_EQ(
            "default.test.undefined"s,
            rb.translate(Locale{ "it" }, "test.undefined"));
    }
}


TEST(CommonResourceBundle, translate_loads_of_stuff_withCurrentLocale)
{
    ResourceBundle rb{ "smack", PROJ_RESOURCE_DIR };


    auto originalLocale = Locale::getCurrent();

    //// Check the default case when the locale is not set yet.
    ASSERT_EQ(
        ""s,
        originalLocale.toString());

    // Resolve with empty locale.  Results in fallback.
    ASSERT_EQ(
        "_trash"s,
        rb.translate("smack.trash"));

    // en exists. Trivial.
    Locale::setCurrent(Locale{ "en" });
    ASSERT_EQ(
        "Trashcan"s,
        rb.translate("smack.trash"));

    // Fallback to en.
    Locale::setCurrent(Locale{ "en", "US" });
    ASSERT_EQ(
        "Trashcan"s,
        rb.translate("smack.trash"));

    // en_GB exists.
    Locale::setCurrent(Locale{ "en", "GB" });
    ASSERT_EQ(
        "Wastebasket"s,
        rb.translate("smack.trash"));

    Locale::setCurrent(originalLocale);
}

TEST(CommonResourceBundle, current_locale_setGet)
{
    // Check the default case when the locale is not set yet.
    ASSERT_EQ(
        ""s,
        Locale::getCurrent().toString());

    Locale::setCurrent(
        Locale{ "de", "DE" });
    ASSERT_EQ(
        "de_DE"s,
        Locale::getCurrent().toString());

    Locale::setCurrent(
        Locale{ "en", "US" });
    ASSERT_EQ(
        "en_US"s,
        Locale::getCurrent().toString());

    Locale::setCurrent(
        Locale{ "it", "US" });
    ASSERT_EQ(
        "it_US"s,
        Locale::getCurrent().toString());
}

TEST(Resources, LocaleEq) {

    Locale enUs1{ "en", "US" };
    Locale enUs2{ "en", "US" };
    ASSERT_EQ(enUs1, enUs2);
    Locale enGb{ "en", "GB" };
    ASSERT_FALSE(enUs1 == enGb);
}

TEST(Resources, Locale) {

    //ASSERT_EQ( "micbinz", std::locale("").name() );
    _locale_t loc = _get_current_locale();

    WCHAR wcBuffer[LOCALE_NAME_MAX_LENGTH];

    int x = GetUserDefaultLocaleName(wcBuffer, LOCALE_NAME_MAX_LENGTH);
    int y = GetLastError();
}
