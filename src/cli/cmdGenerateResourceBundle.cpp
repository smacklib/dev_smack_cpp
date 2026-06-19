/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Implements the command for resource bundle generation.
 *
 * Copyright © 2026 Michael Binz
 */

#include <chrono>
#include <ctime>
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
    const std::string rb_template = R"(/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Generated at ${creation_date_time}.  Do not modify.
 */

#pragma once

#include <initializer_list>
#include <map>
#include <smack_resource_bundle.h>
#include <string>

namespace smack {

#pragma smack format on
inline smack::localisation::ResourceBundleGenerated rb_${bundle_name}{
"${bundle_name}",
"${creation_date_time}",
{
   {
       "",
       ${root_bundle}
   },
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

auto createTocEntry( const smack::localisation::Locale& locale, const smack::util::properties::PropertyMap& map ) -> std::string
{
    return "{\n\"" + locale.toString() + "\",\n" + generateSourceFromMap(map) + "\n}";
}

auto createTocEntry( const std::string& locale, const smack::util::properties::PropertyMap& map ) -> std::string
{
    auto parts = smack::split(locale, "_");

    if (parts.size() == 1) {
        return "  // base locale";
    }
    if (parts.size() == 2) {
        smack::localisation::Locale l(parts[1]);
        return createTocEntry(l, map);
    }
    if (parts.size() == 3) {
        smack::localisation::Locale l(parts[1], parts[2]);
        return createTocEntry(l, map);
    }

    return "    // invalid locale";
}

auto createToc( const std::set<std::filesystem::path>& locales ) -> std::string
{
    std::vector<std::string> tocEntries;

    for ( const auto& locale : locales ) {
        smack::util::properties::PropertyMap map =
            smack::util::properties::loadProperties(locale.string());

        tocEntries.push_back(createTocEntry(locale.stem().string(), map));
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
    std::stringstream ss;
    for (unsigned char c : utf8str) {
        if (c < 128) {
            ss << c;
        } else {
            ss << "\\x" << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(c);
        }
    }
    return ss.str();
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
 *  TODO Add documentation
 */
auto generateSourceFromFile( const std::filesystem::path& filePath ) -> std::string
{
    smack::util::properties::PropertyMap map =
        smack::util::properties::loadProperties(filePath.string());

    return generateSourceFromMap(map);
}

/**
 * Collects all files in the same directory as 'baseBundle' matching the pattern
 * 'baseBundle_<locale>.properties'.
 *
 * @param The 'baseBundle' parameter is a path to the base resource bundle file,
 * for example 'resources/smack.properties'.
 * @return A set of paths to the matching resource bundle files. For example all files
 * 'resources/smack_*.properties'.
 */
auto collectBundles( std::filesystem::path baseBundle ) -> std::set<std::filesystem::path>
{
    std::set<std::filesystem::path> result;

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
const std::string BUNDLE_ROOT              = "root_bundle";
const std::string BUNDLE_TOC               = "toc";
const std::string BUNDLE_CREATION_DATETIME = "creation_date_time";

std::string implGenerateResourceBundle( std::string baseBundleName )
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
            std::make_error_code( std::errc::not_a_directory ) );
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
        BUNDLE_ROOT,
        generateSourceFromFile(inPath),
        result );

    result = replaceProperties(
        BUNDLE_TOC,
        createToc(bundles),
        result );

    return result;
}

} // namespace smack::internal

namespace smack::cli
{

/**
 * The parameter of the base bundle.  For example 'resources/smack.properties'.
 */
int cmdGenerateResourceBundle( std::string baseBundleName )
{
    std::cout <<
        smack::internal::indentGeneratedSource(
            smack::internal::implGenerateResourceBundle(baseBundleName)) << std::endl;

    return EXIT_SUCCESS;
}

} // namespace smack::cli
