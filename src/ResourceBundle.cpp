/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support.
 *
 * Copyright © 2025 Michael Binz
 */

#include "ResourceBundle.h"

#include "smack_util.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace {
    auto loadPropertiesOptionalFile(std::filesystem::path path) -> smack::util::properties::PropertyMap  {
        smack::util::properties::PropertyMap result{};

        try {
            return smack::util::properties::loadProperties(path.u8string());
        }
        catch (const std::exception& )
        {
            return smack::util::properties::PropertyMap{};
        }
    }

#if 0
    /*!
     * Read a file containing key=value definitions.  Does not throw an exception.
     * The returned map allows to access the filename by the key '::filename'. In
     * case the file could not be found the value of this key is "FileNotFound".
     */
    PropertyMap loadPropertiesOptionalFile(const std::string& filename)
    {
        const std::string filenameMetaKey = "::filename";

        try {
            auto result = loadProperties(filename);
            result[filenameMetaKey] = filename;
            return result;
        }
        catch (const std::invalid_argument& e) {
            if (starts_with(e.what(), "FileNotFound")) {
                PropertyMap result;
                result[filenameMetaKey] = "FileNotFound";
                return result;
            }

            throw;
        }
    }
#endif
}

namespace smack::localisation {

using namespace std::string_literals;

auto ResourceBundle::toString() const -> std::string
{
//    return fmt::sprintf("ResourceBundle{%s@%s}", baseName_, location_.c_str());
    return "TODO";
}

auto ResourceBundle::listLocales() const -> std::vector<std::string>
{
    std::vector<std::string> result;

    for (const auto& [key, _] : availableFiles_) {
        result.push_back(key);
    }

    return result;
}

auto ResourceBundle::resolve(const std::string& locale, const std::string& key) -> std::string
{
    // translationMaps_.contains()
    if (translationMaps_.find(locale) == translationMaps_.end()) {
        if (availableFiles_.find(locale) == availableFiles_.end()) {
            // No property file for this locale available.
            return "";
        }

        // Not yet loaded.  Do it...
        translationMaps_.emplace(
            locale, loadPropertiesOptionalFile(availableFiles_.at(locale)));
    }

    // Validate load.
    if (translationMaps_.find(locale) == translationMaps_.end()) {
        throw std::invalid_argument{
            "unexpected:loadPropertiesOptionalFile did not return empty map."};
    }

    auto keyMap = translationMaps_.at(locale);

    if (keyMap.find(key) == keyMap.end()) {
        return ""s;
    }

    return keyMap.at(key);
}

auto ResourceBundle::translate(const Locale& locale, const std::string& key) -> std::string
{
    // Prevent concurrent loading.
    std::lock_guard<std::mutex> lock(mutex_);

    // Handle fully specified locale.
    if (! locale.getLanguage().empty()) {
        auto result = resolve(locale.toString(), key);
        if (! result.empty()) {
            return result;
        }
    }

    // Handle language-only locale.
    {
        auto result = resolve(locale.getLanguage(), key);
        if (! result.empty()) {
            return result;
        }
    }

    {
        // Handle base locale.
        auto result = resolve(""s, key);
        if (! result.empty()) {
            return result;
        }
    }

    return "default." + key;
}

auto ResourceBundle::translate(const std::string& key) -> std::string
{
    auto const currentLocale = Locale::getCurrent();

    return translate(currentLocale, key);
}

auto ResourceBundle::detectLocales() -> void
{
    if (! std::filesystem::exists(location_)) {
        throw std::invalid_argument{"NotFound:"s + location_.u8string()};
    }
    if (! std::filesystem::is_directory(location_)) {
        throw std::invalid_argument{"NoDirectory:"s + location_.u8string()};
    }

    for (const std::filesystem::path& fspath : std::filesystem::directory_iterator(location_)) {

        // Skip non-properties files.
        if (SUFFIX != fspath.extension()) {
            // For "xxx", "xxx.png"
            continue;
        }

        // Is now xxx.properties. Remove suffix.
        std::string base = fspath.stem().u8string();

        // Is now xxx, skip if not starting with basename.
        if (! smack::util::strings::starts_with(base, baseName_)) {
            continue;
        }

        // Is now: basename, or basename_xx_yy.

        // Remove the basename.
        base = base.substr(baseName_.length());

        // Is now the string after basename. Could be empty for the fallback resources.
        // Otherwise, if properly defined, is "_de" or "_en_US" or "";

        // Handle the fallback file.
        if (base.empty()) {
            availableFiles_.insert({base, fspath});
            continue;
        }

        // Remove the leading _.
        if (smack::util::strings::starts_with(base, "_")) {
            base = base.substr(1);
        }

        // Validate the remaining locale definitions.  Expected DE, DE_de.
        auto locales = smack::util::strings::split(base, "_");
        if (locales.size() == 1) {
            ;
        } else if (locales.size() == 2) {
            ;
        } else {
            throw std::invalid_argument("Invalid locale definition: " + base);
        }

        availableFiles_.insert({base, fspath});
    }
}

auto ResourceBundle::hasDefinitions(const Locale& locale) const -> bool
{
    return availableFiles_.find(locale.toString()) != availableFiles_.end();
}

} // namespace smack::localisation
