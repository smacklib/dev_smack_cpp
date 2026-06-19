/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests the ResourceBundle interfaceclass.
 *
 * Copyright © 2026 Michael Binz
 */

#include "gtest/gtest.h"
#include <smack_resource_bundle.h>
#include <smack_resource_bundle_file.h>
#include <rb_smack.h>

#include "test_common.hpp"

#include <functional>
#include <string>
#include <memory>

using Locale = smack::Locale;

namespace {
    const std::filesystem::path PROJ_RESOURCE_DIR =
        smack::test::TEST_DIR / "resources/resourceBundle";
}

/**
 * Holds an existing instance of ResourceBundle.  This is used to wrap
 * the generated resource bundle instance in a way that allows it to be
 * used in the parameterized tests.
 */
class RbHolder : public smack::localisation::ResourceBundle {
    smack::localisation::ResourceBundle* rb_;
public:
    RbHolder(smack::localisation::ResourceBundle& rb) : rb_{&rb}
    {}

    auto tl(const std::string& key) const -> std::string override {
        return rb_->tl(key);
    }

    auto getName() const -> std::string override {
        return rb_->getName();
    }

    auto hasDefinitions(const smack::Locale& locale) const -> bool override {
        return rb_->hasDefinitions(locale);
    }

    auto listLocales() const -> std::set<smack::Locale> override {
        return rb_->listLocales();
    }

    auto toString() const -> std::string override {
        return rb_->toString();
    }
};

struct InterfaceFactoryParam {
    std::string name;
    std::function<std::unique_ptr<smack::localisation::ResourceBundle>()> factory;
};

class T_ResourceBundle : public testing::TestWithParam<InterfaceFactoryParam> {
protected:
    std::unique_ptr<smack::localisation::ResourceBundle> rb;
    Locale savedLocale_;

    void SetUp() override {
        // The factory function is part of the parameter, call it to create the instance.
        rb = GetParam().factory();
        savedLocale_ = Locale::getCurrent();
    }
    void TearDown() override {
        Locale::setCurrent(savedLocale_);
    }
    // Set the locale for the current test.
    void setLocaleByName(const std::string& name) {
        Locale::setCurrent(Locale::makeLocaleFromName(name));
    }
    // Get the test-specific locale.
    auto getLocale() const -> smack::Locale {
        return Locale::getCurrent();
    }
};

TEST_P(T_ResourceBundle, tl_unknown_key) {
    setLocaleByName("");

    std::string unknownKey{ "unknown.key" };

    ASSERT_EQ(
        unknownKey,
        rb->tl(unknownKey));
}

TEST_P(T_ResourceBundle, tl_root) {
    setLocaleByName("");
    ASSERT_EQ(
        "n/a",
        rb->tl("locale"));
    ASSERT_EQ(
        "_trash",
        rb->tl("smack.trash"));
    ASSERT_EQ(
        "_yes",
        rb->tl("smack.yes"));
}

TEST_P(T_ResourceBundle, tl_cn) {
    setLocaleByName("cn");
    ASSERT_EQ(
        getLocale().toString(),
        rb->tl("locale"));
    ASSERT_EQ(
        u8"\xe5\x9e\x83\xe5\x9c\xbe\xe6\xa1\xb6",
        rb->tl("smack.trash"));
    ASSERT_EQ(
        u8"\xe6\x98\xaf",
        rb->tl("smack.yes"));
}

TEST_P(T_ResourceBundle, tl_de) {
    setLocaleByName("de");
    ASSERT_EQ(
        getLocale().toString(),
        rb->tl("locale"));
    ASSERT_EQ(
        "Papierkorb",
        rb->tl("smack.trash"));
    ASSERT_EQ(
        "Ja",
        rb->tl("smack.yes"));
}

TEST_P(T_ResourceBundle, tl_en) {
    setLocaleByName("en");
    ASSERT_EQ(
        getLocale().toString(),
        rb->tl( "locale"));
    ASSERT_EQ(
        "Trashcan",
        rb->tl( "smack.trash"));
    ASSERT_EQ(
        "Yes",
        rb->tl( "smack.yes"));
}

TEST_P(T_ResourceBundle, tl_en_GB) {
    setLocaleByName("en_GB");
    ASSERT_EQ(
        getLocale().toString(),
        rb->tl("locale"));
    ASSERT_EQ(
        "Wastebasket",
        rb->tl("smack.trash"));
    ASSERT_EQ(
        "Yes",
        rb->tl("smack.yes"));  // falls back to en
}

TEST_P(T_ResourceBundle, tl_es) {
    setLocaleByName("es");
    ASSERT_EQ(
        getLocale().toString(),
        rb->tl("locale"));
    ASSERT_EQ(
        u8"S\xc3\xad",
        rb->tl("smack.yes"));
    ASSERT_EQ(
        "_trash",
        rb->tl("smack.trash"));  // falls back to root
}

TEST_P(T_ResourceBundle, tl_fr) {
    setLocaleByName("fr");
    ASSERT_EQ(
        getLocale().toString(),
        rb->tl("locale"));
    ASSERT_EQ(
        "Oui",
        rb->tl("smack.yes"));
    ASSERT_EQ(
        "_trash",
        rb->tl("smack.trash"));  // falls back to root
}

TEST_P(T_ResourceBundle, getName) {
    ASSERT_EQ( "smack", rb->getName());
}

TEST_P(T_ResourceBundle, hasDefinitions) {
    ASSERT_TRUE(rb->hasDefinitions(Locale::makeLocaleFromName("en")));
    ASSERT_TRUE(rb->hasDefinitions(Locale::makeLocaleFromName("de")));
    ASSERT_TRUE(rb->hasDefinitions(Locale::makeLocaleFromName("cn")));
    ASSERT_TRUE(rb->hasDefinitions(Locale::makeLocaleFromName("fr")));
    ASSERT_TRUE(rb->hasDefinitions(Locale::makeLocaleFromName("es")));
    ASSERT_TRUE(rb->hasDefinitions(Locale::makeLocaleFromName("en_GB")));

    ASSERT_FALSE(rb->hasDefinitions(Locale::makeLocaleFromName("ja")));
    ASSERT_FALSE(rb->hasDefinitions(Locale::makeLocaleFromName("pl")));
    ASSERT_FALSE(rb->hasDefinitions(Locale::makeLocaleFromName("en_US")));
}

TEST_P(T_ResourceBundle, listLocales) {

    auto locales = rb->listLocales();

    ASSERT_EQ(6U, locales.size());

    auto it = locales.begin();
    ASSERT_EQ(
        Locale::makeLocaleFromName("cn"),
        *it++);
    ASSERT_EQ(
        Locale::makeLocaleFromName("de"),
        *it++);
    ASSERT_EQ(
        Locale::makeLocaleFromName("en"),
        *it++);
    ASSERT_EQ(
        Locale::makeLocaleFromName("en_GB"),
        *it++);
    ASSERT_EQ(
        Locale::makeLocaleFromName("es"),
        *it++);
    ASSERT_EQ(
        Locale::makeLocaleFromName("fr"),
        *it++);
}

TEST_P(T_ResourceBundle, toString) {
    const auto result = rb->toString();

    // Build the expected comma-separated locale list from listLocales().
    std::string expected;

    for (const auto& locale : rb->listLocales()) {
        if (!expected.empty())
            expected += ", ";
        expected += locale.toString();
    }

    EXPECT_NE(std::string::npos, result.find(expected))
        << "toString() does not contain the comma-separated locale list.\n"
        << "  Expected substring: \"" << expected << "\"\n"
        << "  toString() returned: \"" << result << "\"";
}

INSTANTIATE_TEST_SUITE_P(
    AllInterfaceImplementations,
    T_ResourceBundle,
    testing::Values(

        InterfaceFactoryParam{
            "ResourceBundleGenerated",
            []()
            {
                return std::make_unique<RbHolder>( smack::rb_smack );
            }
        },
        InterfaceFactoryParam{
            "ResourceBundleFile_path",
            []()
            {
                return std::make_unique<smack::localisation::ResourceBundleFile>( PROJ_RESOURCE_DIR / "smack" );
            }
        },
        InterfaceFactoryParam{
            "ResourceBundleFile_string_string",
            []()
            {
                return std::make_unique<smack::localisation::ResourceBundleFile>("smack", PROJ_RESOURCE_DIR);
            }
        }
    ),
    // Custom naming function for test cases
    [](const testing::TestParamInfo<InterfaceFactoryParam>& info) {
        // Google Test will use this name in the test output
        return info.param.name;
    }
);
