/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support — file-based resource bundle.
 *
 * Copyright © 2025-2026 Michael Binz
 */

#pragma once

#include "smack_resource_bundle.h"
#include "smack_properties.hpp"

#include <filesystem>
#include <map>
#include <mutex>
#include <set>
#include <string>

namespace smack::localisation {

/**
 * A resource bundle file system structure for bundlename='name' looks like this:
 *
 * $pos/name.properties
 * $pos/name_de.properties
 * $pos/name_en.properties
 * $pos/name_en_GB.properties
 * $pos/name.properties
 */
class ResourceBundleFile : public ResourceBundle {

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
     * locale name. This is a cache that is filled on demand, thus mutable.
     */
    mutable std::map<std::string, smack::util::properties::PropertyMap> translationMaps_;

    /**
     * Resolves the passed \b key against the the passed \b locale.
     *
     * @param locale A locale in string format: "de_DE", "de", "".
     * @param key The key to resolve.
     */
    auto resolve(const std::string& locale, const std::string& key) const -> std::string;

    /**
     * Detect the available locales.
     */
    auto discoverLocales() -> void;

    /**
     * A mutex that needs to be locked by all non-const public operations.
     */
    mutable std::mutex mutex_;

public:
    /**
     * Create a ResourceBundleFile.
     *
     * @param name The bundle's base name.
     * @param location The position of the resource definition files.
     * @throws std::invalid_argument if the passed location is not a directory.
     * @throws std::invalid_argument if a ResourceBundleFile-file has an invalid
     * locale defintion like "base_xx_yy_zz".
     */
    ResourceBundleFile(const std::string& name, std::filesystem::path location)
        : baseName_{name}
        , location_{location}
    {
        discoverLocales();
    }

    /**
     * Create a ResourceBundleFile.
     *
     * @param bundleBase The position of the root definition file.  This is
     * a path like "my/app/base.properties".  This name is used to resolve the
     * other locale files like "my/app/base_de.properties".
     * @throws std::invalid_argument if a ResourceBundleFile-file has an invalid
     * locale definition like "base_xx_yy_zz".
     */
    ResourceBundleFile(std::filesystem::path bundleBase)
        : baseName_{bundleBase.stem().string()}
        , location_{bundleBase.parent_path()}
    {
        discoverLocales();
    }

    ~ResourceBundleFile() = default;

    /**
     * Translates the passed \b key using \b locale.  If the passed key is not
     * found in the properties, then it is returned with prefix 'default.'.
     *
     * @param locale The locale to be used for the translation, looks like "en_US", "en".
     * @param text The text to translate. Looks like "Camera" or "scopes.camera".
     * @throws invalid::argument if there is a syntax error a resource definition file.
     */
    auto translate(const smack::localisation::Locale& locale, const std::string& key) const -> std::string;

    /**
     * Translates the passed \b key using the current locale that can be accessed by
     * Locale::getCurrent().  If the passed key is not
     * found in the properties, then it is returned with prefix 'default.'.
     *
     * @param locale The locale to be used for the translation, looks like "en_US", "en".
     * @param text The text to translate. Looks like "Camera" or "scopes.camera".
     * @throws invalid::argument if there is a syntax error a resource definition file.
     */
    auto translate(const std::string& key) const -> std::string;
    auto tl(const std::string& key) const -> std::string override
    {
        return translate(key);
    }

    /**
     * Checks if definitions for a \b locale are available.  Only the
     * concrete local that is passed is checked, no resolution is
     * performed.
     *
     * @param locale The locale to check.
     * @return \b true if the locale is available, otherwise \b false.
     */
    auto hasDefinitions(const Locale& locale) const -> bool override;

    /**
     * List the available locales.
     *
     * @return The list of available locales in string format, e.g.
     * "de_DE", "de", "".  The latter represents the root localisation
     * file.
     */
    auto listLocales() const -> std::set<Locale> override;

    /**
     * Debug support.
     */
    auto toString() const -> std::string override;

    /**
     * Returns the base name of the resource bundle.
     * This is the name of the resource bundle without any suffixes.
     */
    auto getName() const -> std::string override
    {
        return baseName_;
    }
};

} // namespace smack::localisation
