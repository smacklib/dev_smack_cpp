/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Property reading.
 *
 * Copyright © 2021 Michael Binz
 */
#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <fstream>

#include "smack_convert.hpp"

namespace smack::util::properties
{

using std::map;
using std::string;
using std::string_view;
using PropertyMap = map<string, string>;
using smack::convert::transform;

PropertyMap loadProperties(string filename);
PropertyMap loadProperties(std::ifstream& file);

class Properties {
private:
    string name_;
    PropertyMap propertyMap_;
    bool fileFound_;

    Properties(PropertyMap map, string name) : propertyMap_{ map }, name_{ name } {
        std::ifstream infile(name);

        fileFound_ = infile.is_open();
    }

public:
    Properties(const string& name) : name_{ name } {
        std::ifstream infile(name);

        fileFound_ = infile.is_open();

        if (fileFound_)
            propertyMap_ = loadProperties(infile);
    }

    bool fileFound() const {
        return fileFound_;
    }

    size_t size() const
    {
        return propertyMap_.size();
    }

    bool hasEntry(const string& key) const
    {
        return propertyMap_.count(key) > 0;
    }

    string get(const string& key, const string& deflt = "") const
    {
        return hasEntry(key) ?
            propertyMap_.at(key) :
            deflt;
    }

    template <typename T>
    auto getAs(string key, T dflt = T{} )->T {

        try {
            T result{};

            transform(
                get(key),
                result);

            return result;
        }
        catch (std::exception&)
        {
            return dflt;
        }
    }
};
} // namespace smack::util::properties
