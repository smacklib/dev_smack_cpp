/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests for resource bundle generator helpers.
 *
 * Copyright © 2026 Michael Binz
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>

#include "cli/cmdGenerateResourceBundle.h"
#include "smack_common.h"
#include <smack_properties.hpp>
#include "test_common.hpp"

TEST(ReplaceProperty, ReplacesAllOccurrences)
{
    const std::string source = "Hello, ${name}! ${name}!";

    auto result = smack::internal::replaceProperties("name", "World", source);

    EXPECT_EQ("Hello, World! World!", result);
}

TEST(ReplaceProperty, LeavesSourceUnchangedWhenPropertyMissing)
{
    const std::string source = "Hello, ${name}!";

    auto result = smack::internal::replaceProperties("other", "World", source);

    EXPECT_EQ(source, result);
}

TEST(ReplaceFirstProperty, ReplacesOnlyFirstOccurrence)
{
    const std::string source = "Hello, ${name} ${name}!";

    auto result_1 = smack::internal::replaceFirstProperty("name", "Donald", source);
    EXPECT_EQ("Hello, Donald ${name}!", result_1);
    auto result_2 = smack::internal::replaceFirstProperty("name", "Duck", result_1);
    EXPECT_EQ("Hello, Donald Duck!", result_2);
}

TEST(ReplaceFirstProperty, LeavesSourceUnchangedWhenPropertyMissing)
{
    const std::string source = "Hello, ${name}!";

    auto result = smack::internal::replaceFirstProperty("other", "World", source);

    EXPECT_EQ(source, result);
}

TEST(ToUnicodeEscapes, EscapesUtf8Bytes) {
    auto result = smack::internal::toUnicodeEscapes("März");

    EXPECT_EQ("M\\xc3\\xa4rz", result);
}

TEST(GenerateSourceFromMap, StringToString) {
    smack::util::properties::PropertyMap m = {
        {"apple", "fruit"},
        {"broccoli", "vegetable"}
    };

    auto result = smack::internal::generateSourceFromMap(m);

    std::string expected =
        "{\n"
        "    {\"apple\", \"fruit\"},\n"
        "    {\"broccoli\", \"vegetable\"}\n"
        "}";

    EXPECT_EQ(expected, result);
}

TEST(GenerateSourceFromMap, Utf8Escaping) {
    smack::util::properties::PropertyMap m = {
        {"january", "January"},
        {"march", "März"}
    };

    auto result = smack::internal::generateSourceFromMap(m);

    std::string expected =
        "{\n"
        "    {\"january\", \"January\"},\n"
        "    {\"march\", \"M\\xc3\\xa4rz\"}\n"
        "}";

    EXPECT_EQ(expected, result);
}

TEST(GenerateSourceFromMap, EmptyMap) {
    using ::testing::MatchesRegex;

    smack::util::properties::PropertyMap m;

    auto result = smack::internal::generateSourceFromMap(m);

    ASSERT_EQ("{\n}", result);
}

TEST(GenerateMapTypeFromMap, StringToString) {
    std::map<std::string, std::string> m;

    auto result = smack::internal::generateMapTypeFromMap(m);

    EXPECT_EQ("std::map<std::string, std::string>", result);
}

TEST(GenerateMapTypeFromMap, UnsignedToString) {
    std::map<unsigned, std::string> m;

    auto result = smack::internal::generateMapTypeFromMap(m);

    EXPECT_EQ("std::map<unsigned int, std::string>", result);
}

TEST(ResourceBundleGenerator, resolve_type_to_string)
{
    ASSERT_EQ("smack::Version", smack::internal::resolve_type<smack::Version>());

    ASSERT_EQ("bool",   smack::internal::resolve_type<bool>());
    ASSERT_EQ("short",  smack::internal::resolve_type<short>());
    ASSERT_EQ("int",    smack::internal::resolve_type<int>());
    ASSERT_EQ("long",   smack::internal::resolve_type<long>());
    ASSERT_EQ("float",  smack::internal::resolve_type<float>());
    ASSERT_EQ("double", smack::internal::resolve_type<double>());
    ASSERT_EQ("std::string", smack::internal::resolve_type<std::string>());
    ASSERT_EQ("std::string", smack::internal::resolve_type<char*>());
    ASSERT_EQ("std::string", smack::internal::resolve_type<const char*>());

    ASSERT_EQ("bool",   smack::internal::resolve_type<const bool&>());
    ASSERT_EQ("int",    smack::internal::resolve_type<const int&>());
    ASSERT_EQ("long",   smack::internal::resolve_type<long&>());
    ASSERT_EQ("double", smack::internal::resolve_type<const double&>());
    ASSERT_EQ("std::string", smack::internal::resolve_type<const std::string&>());
    ASSERT_EQ("std::string", smack::internal::resolve_type<std::string&>());
}

TEST(CollectBundles, CollectsSiblingLocaleBundles)
{
    const auto testDir = std::filesystem::temp_directory_path() / "smack_collect_bundles";
    std::filesystem::remove_all(testDir);
    std::filesystem::create_directories(testDir);

    const auto baseBundle = testDir / "smack.properties";
    const auto englishBundle = testDir / "smack_en.properties";
    const auto britishBundle = testDir / "smack_en_GB.properties";
    // Not found, since the prefix is not 'smack'.
    const auto unrelatedBundle = testDir / "other_en.properties";
    // Not found, since the extension is not .properties.
    const auto wrongExtension = testDir / "smack_de.txt";

    std::ofstream(baseBundle.string()).put('\n');
    std::ofstream(englishBundle.string()).put('\n');
    std::ofstream(britishBundle.string()).put('\n');
    std::ofstream(unrelatedBundle.string()).put('\n');
    std::ofstream(wrongExtension.string()).put('\n');

    const auto result = smack::internal::collectBundles(baseBundle);

    const std::set<std::filesystem::path> expected = {
        englishBundle,
        britishBundle,
    };

    EXPECT_EQ(expected, result);

    std::filesystem::remove_all(testDir);
}

TEST(ImplGenerateResourceBundle, ThrowsForNonexistentFile)
{
    EXPECT_THROW(
        smack::internal::implGenerateResourceBundle("/nonexistent/path/no.properties"),
        std::filesystem::filesystem_error);
}

TEST(ImplGenerateResourceBundle, ThrowsForNonPropertiesExtension)
{
    const auto testDir = std::filesystem::temp_directory_path() / "smack_impl_gen_rb";
    std::filesystem::create_directories(testDir);
    const auto notProps = testDir / "bundle.txt";
    std::ofstream(notProps.string()) << "key=value\n";

    EXPECT_THROW(
        smack::internal::implGenerateResourceBundle(notProps.string()),
        std::filesystem::filesystem_error);

    std::filesystem::remove_all(testDir);
}

TEST(IndentGeneratedSource, CopiesVerbatimWhenNoPragma)
{
    const std::string source =
        "line one\n"
        "line two\n"
        "line three";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ(source, result);
}

TEST(IndentGeneratedSource, RemovesPragmaMarkers)
{
    const std::string source =
        "before\n"
        "#pragma smack format on\n"
        "#pragma smack format off\n"
        "after";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ("before\nafter", result);
}

TEST(IndentGeneratedSource, IndentsAfterOpenBrace)
{
    const std::string source =
        "#pragma smack format on\n"
        "{\n"
        "content\n"
        "}";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ("{\n    content\n    }", result);
}

TEST(IndentGeneratedSource, DecreasesLevelAfterCloseBraceLine)
{
    const std::string source =
        "#pragma smack format on\n"
        "{\n"
        "inner\n"
        "}\n"
        "after";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ("{\n    inner\n    }\nafter", result);
}

TEST(IndentGeneratedSource, NestedBraces)
{
    const std::string source =
        "#pragma smack format on\n"
        "outer {\n"
        "inner {\n"
        "content\n"
        "}\n"
        "}";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ(
        "outer {\n"
        "    inner {\n"
        "        content\n"
        "        }\n"
        "    }",
        result);
}

TEST(IndentGeneratedSource, SingleLineBraceEntryNoLevelChange)
{
    // A line like {"key", "value"} ends with '}' and decreases the level
    // for the following line, but the line itself is printed at the current level.
    const std::string source =
        "#pragma smack format on\n"
        "map = {\n"
        "{\"apple\", \"fruit\"},\n"
        "{\"broccoli\", \"vegetable\"}\n"
        "};";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ(
        "map = {\n"
        "    {\"apple\", \"fruit\"},\n"
        "    {\"broccoli\", \"vegetable\"}\n"
        "};",
        result);
}

TEST(IndentGeneratedSource, LevelDoesNotGoBelowZero)
{
    const std::string source =
        "#pragma smack format on\n"
        "}\n"
        "line";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ("}\nline", result);
}

TEST(IndentGeneratedSource, TrimsLeadingAndTrailingWhitespaceInFormattedSection)
{
    const std::string source =
        "#pragma smack format on\n"
        "  trimmed line  \n"
        "   another   ";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ("trimmed line\nanother", result);
}

TEST(IndentGeneratedSource, VerbatimBeforeAndFormattedAfterPragma)
{
    const std::string source =
        "verbatim   line\n"
        "#pragma smack format on\n"
        "  formatted  ";

    auto result = smack::internal::indentGeneratedSource(source);

    EXPECT_EQ("verbatim   line\nformatted", result);
}

TEST(IndentGeneratedSource, UsesCustomIndent)
{
    const std::string source =
        "#pragma smack format on\n"
        "{\n"
        "content\n"
        "}";

    auto result = smack::internal::indentGeneratedSource(source, "\t");

    EXPECT_EQ("{\n\tcontent\n\t}", result);
}

TEST(ImplGenerateResourceBundle, GeneratesValidCppSourceForBaseBundle)
{
    namespace fs = std::filesystem;

    const fs::path baseBundle =
        smack::test::makeResourcePath("resourceBundle/good/smack.properties");
    const std::string result =
        smack::internal::implGenerateResourceBundle(baseBundle.string());

    // The base map must be present.
    EXPECT_NE(
        std::string::npos,
        result.find("smack.yes"));
    EXPECT_NE(
        std::string::npos,
        result.find("smack.trash"));
    // Locale entries from the sibling bundles must appear in the table of contents.
    EXPECT_NE(
        std::string::npos,
        result.find("de"));
    EXPECT_NE(
        std::string::npos,
        result.find("en"));
}
