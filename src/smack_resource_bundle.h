/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support.
 *
 * Copyright � 2025 Michael Binz
 */

#pragma once

#include "smack_locale.h"
#include "smack_properties.hpp"

#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace smack::localisation {

/**
 * A resource bundle file system structure for bundlename='name' looks like this:
 *
 * $pos/name.properties
 * $pos/name_deu.properties
 * $pos/name_eng.properties
 * $pos/name_eng_GB.properties
 * $pos/name_cmn.properties
 */
class ResourceBundle {

    static inline std::filesystem::path SUFFIX{".properties"};

    /**
     * Holds the base name of the bundle.
     * This corresponds to the fallback bundle.
     */
    std::string baseName_;

    /**
     * The filesystem position of the bundle files.
     */
    std::filesystem::path location_;

    /**
     * Maps string-locales like "DE_de" or "DE" to the respective definition
     * file.  Contains a mapping like "" for the fallback file if one exists.
     */
    std::map<std::string, const std::filesystem::path> availableFiles_;

    /**
     * Holds the translation maps for the locales.  Key is the stringified
     * locale name.
     */
    std::map<std::string, smack::util::properties::PropertyMap> translationMaps_;

    /**
     * Resolves the passed \b key against the the passed \b locale.
     *
     * @param locale A locale in string format: "de_DE", "de", "".
     * @param key The key to resolve.
     */
    auto resolve(const std::string& locale, const std::string& key) -> std::string;

    /**
     * Detect the available locales.
     */
    auto detectLocales() -> void;

    /**
     * A mutex that needs to be locked by all non-const public operations.
     */
    mutable std::mutex mutex_;

public:
    /**
     * Create a ResourceBundle.
     *
     * @param name The bundle's base name.
     * @param location The position of the resource definition files.
     * @throws std::invalid_argument if the passed location is not a directory.
     * @throws std::invalid_argument if a ResourceBundle-file has an invalid
     * locale defintion like "base_xx_yy_zz".
     */
    ResourceBundle(const std::string& name, std::filesystem::path location)
        : baseName_{name}
        , location_{location}
    {
        detectLocales();
    }

    ~ResourceBundle() = default;

    /**
     * Translates the passed \b key using \b locale.  If the passed key is not
     * found in the properties, then it is returned with prefix 'default.'.
     *
     * @param locale The locale to be used for the translation, looks like "en_US", "en".
     * @param text The text to translate. Looks like "Camera" or "scopes.camera".
     * @throws invalid::argument if there is a syntax error a resource definition file.
     */
    auto translate(const smack::localisation::Locale& locale, const std::string& key) -> std::string;

    /**
     * Translates the passed \b key using the current locale that can be accessed by
     * Locale::getCurrent().  If the passed key is not
     * found in the properties, then it is returned with prefix 'default.'.
     *
     * @param locale The locale to be used for the translation, looks like "en_US", "en".
     * @param text The text to translate. Looks like "Camera" or "scopes.camera".
     * @throws invalid::argument if there is a syntax error a resource definition file.
     */
    auto translate(const std::string& key) -> std::string;

    /**
     * Checks if definitions for a \b locale are available.  Only the
     * concrete local that is passed is checked, no resolution is
     * performed.
     *
     * @param locale The locale to check.
     * @return \b true if the locale is available, otherwise \b false.
     */
    auto hasDefinitions(const Locale& locale) const -> bool;

    /**
     * List the available locales.
     *
     * @return The list of available locales in string format, e.g.
     * "de_DE", "de", "".  The latter represents the root localisation
     * file.
     */
    auto listLocales() const -> std::vector<std::string>;

    /**
     * Debug support.
     */
    auto toString() const -> std::string;
};

} // namespace smack::localisation
