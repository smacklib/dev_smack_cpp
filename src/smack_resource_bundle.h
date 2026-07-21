/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support.
 *
 * Copyright © 2025-2026 Michael Binz
 */

#pragma once

#include "smack_locale.h"

#include <set>
#include <string>

namespace smack::localisation {

class ResourceBundle {
public:
    virtual ~ResourceBundle() = default;

/**
 * Translates the passed \b key using the current locale that can be accessed by
 * Locale::getCurrent().  If the passed key is not
 * found in the properties, then it is returned with prefix 'default.'.
 *
 * @param locale The locale to be used for the translation, looks like "en_US", "en".
 * @param text The text to translate. Looks like "Camera" or "scopes.camera".
 * @throws invalid::argument if there is a syntax error a resource definition file.
 */
virtual auto tl(const std::string& key) const -> std::string = 0;

/**
 * Get the name of the resource bundle.
 * This is the base name of the resource bundle without any suffixes.
 */
virtual auto getName() const -> std::string = 0;

/**
 * Checks if definitions for a \b locale are available.  The
 * passed locale is checked against the actually available locales.
 * For the root locale (empty locale) this returns false.
 *
 * @param locale The locale to check.
 * @return \b true if the locale is available, otherwise \b false.  For the root
 * locale this returns false.
 */
virtual auto hasDefinitions(const smack::Locale& locale) const -> bool = 0;

/**
 * List the available locales.
 *
 * @return The list of available locales in string format, e.g.
 * "de_DE", "de".  The list only contains the locales that are
 * available in the resource bundle, not the root locale.
 */
virtual auto listLocales() const -> std::set<smack::Locale> = 0;

/**
 * Debug support.
 */
virtual auto toString() const -> std::string = 0;
};

} // namespace smack::localisation

#include "smack_resource_bundle_generated.h"
#include "smack_resource_bundle_file.h"
