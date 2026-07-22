/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Implements the command for resource bundle generation.
 *
 * Copyright © 2026 Michael Binz
 */

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <smack_cli.hpp>
#include <smack_common.h>
#include <smack_locale.h>
#include <smack_system.h>
#include <smack_util.hpp>
#include <smack_properties.hpp>
#include "cmdGenerateResourceBundle.h"

namespace smack::internal
{

using Locale = smack::Locale;

    const std::string rb_template = R"(/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Generated at ${creation_date_time}.  Do not modify.
 */

#pragma once

#include <initializer_list>
#include <map>
#include <string>

#include <smack_resource_bundle_generated.h>

namespace smack {

#pragma smack format on
inline smack::localisation::ResourceBundleGenerated rb_${bundle_name}{
"${bundle_name}",
"${creation_date_time}",
{
   ${toc}
}
#pragma smack format off
};

} // namespace smack
)";

auto indentGeneratedSource( const std::string& source, const std::string& indent ) -> std::string
{
    static const std::string PRAGMA_ON  = "#pragma smack format on";
    static const std::string PRAGMA_OFF = "#pragma smack format off";

    std::istringstream stream( source );
    std::ostringstream result;
    std::string line;
    bool formatting = false;
    int indentationLevel = 0;
    bool firstLine = true;

    while ( std::getline( stream, line ) )
    {
        // Compute trimmed version for pragma detection and formatted output.
        std::string trimmed = smack::trim(line);

        if ( trimmed == PRAGMA_ON ) {
            formatting = true;
            continue;
        }
        if ( trimmed == PRAGMA_OFF ) {
            formatting = false;
            continue;
        }

        if ( !firstLine )
            result << "\n";
        firstLine = false;

        if ( !formatting ) {
            result << line;
            continue;
        }

        // Formatted section: apply indentation rules.
        char lastChar = trimmed.empty() ? '\0' : trimmed.back();

        for ( int i = 0; i < indentationLevel; ++i )
            result << indent;
        result << trimmed;

        if ( lastChar == '{' )
            ++indentationLevel;
        else if ( lastChar == '}' && indentationLevel > 0 )
            --indentationLevel;
    }

    return result.str();
}

auto createTocEntry( const Locale& locale, const smack::util::properties::PropertyMap& map ) -> std::string
{
    return "{\n\"" + locale.toString() + "\",\n" + generateSourceFromMap(map) + "\n}";
}

/** TODO smack::util candidate
 * Removes the passed prefix from the passed buffer and returns the modified string.
 *
 * Checks if the `buffer` string starts with the `prefix` string.
 * If it does, it returns a new string that is the `buffer` with the `prefix` removed
 * from its beginning. If the `buffer` does not start with the `prefix`, or if the
 * `prefix` is longer than the `buffer`, the original `buffer` is returned unchanged.
 *
 * @param prefix The string to be removed from the beginning of the buffer.
 * @param buffer The string from which the prefix should be removed.
 * @return A new string with the prefix removed, or the original buffer if the prefix
 *         was not found at the beginning.
 */
std::string removePrefix(const std::string prefix, const std::string& buffer) {

    if (buffer.length() < prefix.length()) {
        // Buffer is shorter than the prefix, so it cannot start with it.
        return buffer;
    }

    if (smack::starts_with(buffer, prefix)) {
        return buffer.substr(prefix.length());
    }

    return buffer;
}

auto createToc(
    const std::string& bundleName,
    const std::set<std::filesystem::path>& bundles ) -> std::string
{
    std::vector<std::string> tocEntries;

    for ( const auto& bundle : bundles ) {
        smack::util::properties::PropertyMap map =
            smack::util::properties::loadProperties(bundle.string());

        auto localeName =
            smack::trim(
                removePrefix(
                    bundleName,
                    bundle.stem().string()),
                "_");

        tocEntries.push_back(
            createTocEntry(
                Locale::makeLocaleFromName( localeName ),
                 map ) );
    }

    return smack::concat(tocEntries, ",\n");
}

auto replaceProperties(
    const std::string& name,
    const std::string& value,
    const std::string& source ) -> std::string
{
    const std::string propertyName = "${" + name + "}";

    std::string result{ source };
    std::string::size_type pos = 0;

    while ((pos = result.find(propertyName, pos)) != std::string::npos) {
        result.replace(pos, propertyName.size(), value);
        pos += value.size();
    }

    return result;
}

auto replaceFirstProperty(
    const std::string& name,
    const std::string& value,
    const std::string& source ) -> std::string
{
    const std::string propertyName = "${" + name + "}";

    std::string result{ source };
    const std::string::size_type pos = result.find(propertyName);

    if (pos != std::string::npos) {
        result.replace(pos, propertyName.size(), value);
    }

    return result;
}

auto toUnicodeEscapes( const std::string& utf8str ) -> std::string
{
    std::stringstream result;

    for (unsigned char c : utf8str) {
        if (c < 128) {
            result << c;
        } else {
            result << "\\x" << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(c);
        }
    }

    return result.str();
}

auto generateSourceFromMap( const smack::util::properties::PropertyMap& map ) -> std::string
{
    std::ostringstream result;
    result << "{\n";

    auto it = map.begin();
    while (it != map.end()) {
        result << "    {\"" << it->first << "\", \""
               << toUnicodeEscapes(it->second) << "\"";

        ++it;
        result << (it != map.end() ? "},\n" : "}\n");
    }

    result << "}";

    return result.str();
}

/**
 * Collects all files in the same directory as 'baseBundle' matching the pattern
 * 'baseBundle_<locale>.properties'.  baseBundle itself is also included in the
 * result set.
 *
 * @param The 'baseBundle' parameter is a path to the base resource bundle file,
 * for example 'resources/smack.properties'.
 * @return A set of paths to the matching resource bundle files. For example all files
 * 'resources/smack_*.properties'.
 */
auto collectBundles( const std::filesystem::path& baseBundle ) -> std::set<std::filesystem::path>
{
    std::set<std::filesystem::path> result{baseBundle};

    const auto parentDir = baseBundle.parent_path().empty()
        ? std::filesystem::path{"."}
        : baseBundle.parent_path();
    const auto expectedPrefix =
        baseBundle.stem().string() + "_";
    const auto expectedExtension =
        baseBundle.extension();

    for ( const auto& entry : std::filesystem::directory_iterator( parentDir ) ) {
        if ( !entry.is_regular_file() ) {
            continue;
        }

        const auto filePath = entry.path();
        if ( filePath.extension() != expectedExtension ) {
            continue;
        }

        const auto fileName = filePath.filename().string();
        if (! smack::starts_with(fileName, expectedPrefix)) {
            continue;
        }

        result.insert( filePath );
    }

    return result;
}

// Bundle base name.
const std::string BUNDLE_NAME              = "bundle_name";
const std::string BUNDLE_TOC               = "toc";
const std::string BUNDLE_CREATION_DATETIME = "creation_date_time";

std::string implGenerateResourceBundle( const std::string& baseBundleName )
{
    std::filesystem::path inPath{ baseBundleName };

    if ( !std::filesystem::exists( inPath ) ) {
        throw std::filesystem::filesystem_error(
            "File not found",
            inPath,
            std::make_error_code( std::errc::no_such_file_or_directory ) );
    }

    if ( !std::filesystem::is_regular_file( inPath ) ) {
        throw std::filesystem::filesystem_error(
            "Not a regular file",
            inPath,
            std::make_error_code( std::errc::not_supported ) );
    }

    if ( inPath.extension() != ".properties" ) {
        throw std::filesystem::filesystem_error(
            "Not a .properties file",
            inPath,
            std::make_error_code( std::errc::not_supported ) );
    }

    auto bundles = collectBundles( inPath );

    // The base name of the resource bundle.
    std::string bundleName = inPath.stem().string();

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char timeBuf[32]{};
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string result = rb_template;

    result = replaceProperties(
        BUNDLE_CREATION_DATETIME,
        std::string(timeBuf),
        result );

    result = replaceProperties(
        BUNDLE_NAME,
        bundleName,
        result );

    result = replaceProperties(
        BUNDLE_TOC,
        createToc( bundleName, bundles ),
        result );

    return indentGeneratedSource(result);
}

} // namespace smack::internal

namespace smack::cli
{

/**
 * The parameter of the base bundle.  For example 'resources/smack.properties'.
 */
int cmdGenerateResourceBundle( const std::string& baseBundleName )
{
    std::cout <<
        smack::internal::implGenerateResourceBundle(baseBundleName) << std::endl;

    return EXIT_SUCCESS;
}

int cmdGenerateResourceBundleToDirectory( const std::string& baseBundleName, const std::string& outputDirectory )
{
    const std::filesystem::path bundlePath(baseBundleName);
    const std::string bundleName = bundlePath.stem().string();
    const std::filesystem::path outputFile =
        std::filesystem::path(outputDirectory) / ("rb_" + bundleName + ".h");

    std::ofstream out(outputFile);

    if (!out) {
        throw std::runtime_error("Cannot open output file: " + outputFile.string());
    }

    out <<
        smack::internal::implGenerateResourceBundle(baseBundleName) << std::endl;

    return EXIT_SUCCESS;
}

} // namespace smack::cli
