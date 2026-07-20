/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Command definitions.
 *
 * Copyright © 2026 Michael Binz
 */

#pragma once

#include <filesystem>
#include <map>
#include <set>
#include <string>

#include <smack_convert.hpp>
#include <smack_properties.hpp>
#include <smack_system.h>


namespace smack::internal
{
    /**
     * Accepts a multi-line string and returns a result that is indented according
     * to the following rules:
     *
     * - The input lines are copied verbatim to the result until a line '#pragma smack format on' is encountered.
     *   After that line, the indentation rules below are applied until a line '#pragma smack format off' is encountered. These
     *   markers are removed from the result.
     *
     * Formatting rules:
     * - Each line in the input string is trimmed of leading and trailing whitespace.
     * - An indentationLevel is internally maintained.  The initial indentationLevel is 0.
     * - Indentation is performed by concatenating indentationLevel times the specified indent
     *   string at the beginning of each result line.
     * - Each line that ends into a '{' results in an increase of the indentationLevel by one for the following lines.
     * - A line that ends into a '}' results in a decrease of the indentationLevel by one for the following lines.
     * - Lines that do not end with '{' or '}' maintain the current indentationLevel.
     * - The minimum indentation level is 0.  If a line ends with '}' and the
     *   current indentationLevel is 0, the indentationLevel remains 0.
     */
    auto indentGeneratedSource( const std::string& source, const std::string& indent = "    " ) -> std::string;

    /**
     * Replaces all occurrences of a named property in the source string with
     * the given value.  Properties in the input string are expected to be in
     * the format ${propertyName}.
     *
     * For example, if the source string is "Hello, ${name}!" the passed
     * parameters are name="name and value="World", the result will
     * be "Hello, World!".
     */
    auto replaceProperties(
        const std::string& name,
        const std::string& value,
        const std::string& source ) -> std::string;

    /**
     * Replaces the first occurrence of a named property in the source string
     * with the given value.  Properties in the input string are expected to be
     * in the format ${propertyName}.
     *
     * For example, if the source string is "Hello, ${name} ${name}!" the passed
     * parameters are name="name and value="World", the result will
     * be "Hello, World ${name}!".
     */
    auto replaceFirstProperty(
        const std::string& name,
        const std::string& value,
        const std::string& source ) -> std::string;

    /**
     * Converts an UTF-8 std::string to an escaped string where non-ASCII bytes
     * are represented as \xHH escape sequences.
     */
    auto toUnicodeEscapes( const std::string& utf8str ) -> std::string;

    /**
     * Generates a literal definition of the passed map in C++ source code.
     *
     * smack::util::properties::PropertyMap m = {
     *   {"apple", "fruit"},
     *   {"broccoli", "vegetable"}
     * };
     *
     * auto result = smack::internal::generateSourceFromMap(m);
     *
     *  =>
     *    {
     *        {"apple", "fruit"},
     *        {"broccoli", "vegetable"}
     *    };
     */
    auto generateSourceFromMap( const smack::util::properties::PropertyMap& map ) -> std::string;

    auto collectBundles( const std::filesystem::path& baseBundle ) -> std::set<std::filesystem::path>;

    auto implGenerateResourceBundle( const std::string& baseBundleName ) -> std::string;
}

namespace smack::cli
{
    auto cmdGenerateResourceBundle( const std::string& path ) -> int;
    auto cmdGenerateResourceBundleToFile( const std::string& path, const std::string& outputFile ) -> int;
    auto cmdGenerateResourceBundleToDirectory( const std::string& path, const std::string& outputDirectory ) -> int;
} // namespace smack::cli
