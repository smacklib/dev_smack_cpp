/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2019-2025 Michael Binz
 */
#include <gtest/gtest.h>

#include <filesystem>
#include <set>
#include <algorithm>
#include <string>
#include <vector>

#if WIN32
// Disables warnings when windows.h is included.
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <Windows.h>
#include <WinNls.h>
#endif

#include <smack_properties.hpp>
#include <smack_resource_bundle.h>
#include <smack_util.hpp>
#include "test_common.hpp"

using Locale = smack::localisation::Locale;
using PropertyMap = smack::util::properties::PropertyMap;
using ResourceBundleFile = smack::localisation::ResourceBundleFile;
using std::string;
using std::vector;
using namespace std::string_literals;
namespace strings = smack::util::strings;
namespace properties = smack::util::properties;

namespace {
    const std::filesystem::path PROJ_RESOURCE_DIR =
        smack::test::TEST_DIR / "resources/resourceBundle/good";
}

TEST(ResourceBundleFile, resourceDirExists)
{
    ASSERT_TRUE(
        std::filesystem::exists(PROJ_RESOURCE_DIR));
}

TEST(ResourceBundleFile, toString)
{
    ResourceBundleFile rb{ "smack", PROJ_RESOURCE_DIR };

    ASSERT_EQ("ResourceBundleFile{smack@" + PROJ_RESOURCE_DIR.string() + "}", rb.toString());
}

TEST(ResourceBundleFile, listLocales)
{
    ResourceBundleFile rb{ "smack", PROJ_RESOURCE_DIR };

    auto locales = rb.listLocales();

    ASSERT_EQ(7U, locales.size());

    // std::set<Locale> is already sorted.
    auto it = locales.begin();
    ASSERT_EQ(Locale{},           *it++);
    ASSERT_EQ(Locale{"cn"},       *it++);
    ASSERT_EQ(Locale{"de"},       *it++);
    ASSERT_EQ(Locale{"en"},       *it++);
    ASSERT_EQ(Locale("en", "GB"), *it++);
    ASSERT_EQ(Locale{"es"},       *it++);
    ASSERT_EQ(Locale{"fr"},       *it++);
}

TEST(ResourceBundleFile, translate_enUs)
{
    Locale en_US{ "en", "US" };
    ResourceBundleFile rb{ "smack", PROJ_RESOURCE_DIR };

    // We have *no* US-language.
    ASSERT_FALSE(rb.hasDefinitions(en_US));
    // Thus we resolve to en.
    ASSERT_EQ("Yes"s, rb.translate(en_US, "smack.yes"));
}

auto translate_loads_of_stuff( const ResourceBundleFile& rb ) -> void
{
    const string LNG = "ISO-639-1"s;
    const string CNT = "ISO-3166-2"s;

    {
        Locale cn{ "cn" };
        ASSERT_EQ(
            "cn"s,
            rb.translate(cn, LNG));

        ASSERT_EQ(
            u8"是"s,
            rb.translate(cn, "smack.yes"));
        ASSERT_EQ(
            u8"垃圾桶"s,
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

TEST(ResourceBundleFile, translate_loads_of_stuff)
{
    ResourceBundleFile rb{ "smack", PROJ_RESOURCE_DIR };

    translate_loads_of_stuff(rb);
}
TEST(ResourceBundleFile, translate_loads_of_stuff2)
{
    ResourceBundleFile rb{ PROJ_RESOURCE_DIR / "smack.properties" };

    translate_loads_of_stuff(rb);
}

TEST(ResourceBundleFile, translate_loads_of_stuff_with_varying_locale)
{
    ResourceBundleFile rb{ "smack", PROJ_RESOURCE_DIR };

    auto originalLocale = Locale::getCurrent();

    ASSERT_FALSE( Locale::getCurrent().isEmpty() );

    Locale::setCurrent(Locale{});

    ASSERT_TRUE(
        Locale::getCurrent().isEmpty());

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

    // Cleanup.
    Locale::setCurrent(originalLocale);
}
