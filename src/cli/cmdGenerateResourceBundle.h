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
#include <smack_convert.hpp>
#include <smack_properties.hpp>

#include <string>

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
     * Resolves the passed type T to its C++ type name as a string.
     */
    template<typename T>
    auto resolve_type() -> std::string
    {
        using U = std::decay_t<T>;
        if constexpr (std::is_same_v<U, bool>)
            return "bool";
        else if constexpr (std::is_same_v<U, short>)
            return "short";
        else if constexpr (std::is_same_v<U, int>)
            return "int";
        else if constexpr (std::is_same_v<U, long>)
            return "long";
        else if constexpr (std::is_same_v<U, double>)
            return "double";
        else if constexpr (std::is_same_v<U, float>)
            return "float";
        else if constexpr (
            std::is_same_v<U, char*> || std::is_same_v<U, const char*> || std::is_same_v<U, std::string>)
            return "std::string";
        else {
            return smack::system::demangle<U>();
        }
    }

    /**
     * Converts an UTF-8 std::string to an escaped string where non-ASCII bytes
     * are represented as \xHH escape sequences.
     */
    auto toUnicodeEscapes( const std::string& utf8str ) -> std::string;

    /**
     * Creates a C++ source code string containing
     * the literal definition of the type of the passed map.
     */
    template <typename K, typename V>
    auto generateMapTypeFromMap( std::map<K,V> ) -> std::string
    {
        return std::string("std::map<")
            + smack::internal::resolve_type<K>()
            + ", "
            + smack::internal::resolve_type<V>()
            + ">";
    }

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

    auto collectBundles( std::filesystem::path baseBundle ) -> std::set<std::filesystem::path>;

    auto implGenerateResourceBundle( std::string baseBundleName ) -> std::string;
}

namespace smack::cli
{
    auto cmdGenerateResourceBundle( std::string path ) -> int;
} // namespace smack::cli
