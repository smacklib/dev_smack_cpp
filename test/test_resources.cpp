/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2019-2025 Michael Binz
 */
#include <gtest/gtest.h> // googletest header file

#include <set>
#include <string>
#include <filesystem>

#include <smack_resource_bundle.h>
#include <smack_properties.hpp>
#include <smack_util.hpp>
#include "test_common.hpp"

using Locale = smack::localisation::Locale;
using PropertyMap = smack::util::properties::PropertyMap;
using ResourceBundle = smack::localisation::ResourceBundle;
namespace strings = smack::util::strings;
namespace properties = smack::util::properties;

namespace
{
    /**
     * Makes a locale Locale{language,country} from a string language_country.
     */
    auto localeFromName( std::string name ) -> Locale
    {
        auto split = strings::split( name, "_" );

        if (split.size() == 2) {
            return Locale{
                split.at( 0 ),
                split.at( 1 ) };
        } else {
            return Locale{split.at(0)};
        }
    }

}

TEST(Resources, LocalizationFallbackExists){
    ASSERT_TRUE(std::filesystem::exists("../../resources/localization/cmob.properties"));
}

TEST(Resources, LocalizationFallbackEqualsEnglish){
    std::string source = "../../resources/localization/cmob_eng.properties";
    std::string target = "../../resources/localization/cmob.properties";

    PropertyMap sourceMap = properties::loadProperties(source);
    PropertyMap targetMap = properties::loadProperties(target);

    for (auto [srcKey, srcTranslation] : sourceMap)
    {
        ASSERT_TRUE(targetMap.find(srcKey)->second == srcTranslation);
    }
}

TEST(Resources, LocalizationPropertiesExist){

    std::string location = "../../resources/localization/";

    PropertyMap sourceProperties = properties::loadProperties(location + "cmob.properties");

    for (const std::filesystem::path& fspath : std::filesystem::directory_iterator(location))
    {
        PropertyMap targetProperties = properties::loadProperties(fspath.string());

        for (auto i : sourceProperties){
            if(! strings::starts_with(i.first, "cmob.scope")){
                ASSERT_TRUE(targetProperties.find(i.first)->second != "" && targetProperties.find(i.first) != targetProperties.end());
            }
        }
    }
}
