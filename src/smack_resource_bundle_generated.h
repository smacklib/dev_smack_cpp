/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support — generated resource bundle.
 *
 * Copyright © 2025-2026 Michael Binz
 */

#pragma once

#include "smack_resource_bundle.h"
#include "smack_util.hpp"

#include <map>
#include <set>
#include <string>

namespace smack::localisation {

/**
 * A class implemented by the resource bundle generation code.
 */
class ResourceBundleGenerated : public ResourceBundle {

    ResourceBundleGenerated(const ResourceBundleGenerated&) = delete;
    ResourceBundleGenerated& operator=(const ResourceBundleGenerated&) = delete;
    ResourceBundleGenerated(ResourceBundleGenerated&&) = delete;
    ResourceBundleGenerated& operator=(ResourceBundleGenerated&&) = delete;

protected:

const std::string baseName_;
const std::string creationDateTime_;

const std::map<std::string, std::map<std::string, std::string>> tableOfContents_;

public:

    ResourceBundleGenerated(
        std::string baseName,
        std::string creationDateTime,
        std::map<std::string, std::map<std::string, std::string>> tableOfContents)
        : baseName_{std::move(baseName)}
        , creationDateTime_{std::move(creationDateTime)}
        , tableOfContents_{std::move(tableOfContents)}
    {
    }

    auto getName() const -> std::string override
    {
        return baseName_;
    }

    /**
     * See ResourceBundle::hasDefinitions() for details.
     */
    auto hasDefinitions(const smack::Locale& locale) const
        -> bool override
    {
        return listLocales().count(locale) > 0;
    }

    /**
     * See ResourceBundle::listLocales() for details.
     */
    auto listLocales() const -> std::set<smack::Locale> override
    {
        std::set<smack::Locale> result;

        for (const auto& [localeName, _] : tableOfContents_) {
            result.insert(smack::Locale::makeLocaleFromName(localeName));
        }

        // Remove the root locale from the list.
        result.erase(smack::Locale{});

        return result;
    }

    /**
     * See ResourceBundle::tl() for details.
     */
    std::string tl(const std::string& key) const override
    {
        smack::Locale currentLocale = smack::Locale::getCurrent();

        while ( true )
        {
            std::string localeName = currentLocale.toString();

            if (tableOfContents_.count(localeName) > 0) {
                const auto& resourceBundle = tableOfContents_.at(localeName);
                if (resourceBundle.count(key) > 0) {
                    return resourceBundle.at(key);
                }
            }

            if (currentLocale.isEmpty()) {
                break;
            }

            currentLocale = currentLocale.getParent();
        }

        // Finally return the key itself if no translation is found.
        return key;
    }

    /**
     * See ResourceBundle::toString() for details.
     */
    auto toString() const -> std::string override
    {
        std::string result = "ResourceBundle:";
        result += getName();
        result +=  "\n{ ";

        auto locales = listLocales();

        result += smack::concat(
            smack::transform(
                locales,
                [&](const smack::Locale& locale) {
                    return locale.toString();
                } ),
            ", ");

        result += " }\nCreated: " + creationDateTime_;

        return result;
    }
};

} // namespace smack::localisation
