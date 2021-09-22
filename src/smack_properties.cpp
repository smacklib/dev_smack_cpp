/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Property reading.
 *
 * Copyright © 2021 Michael Binz
 */

#include "smack_properties.hpp"
#include "smack_util.hpp"

#include <fstream>


namespace
{
template<typename T, typename U>
bool nextLine(T& stream, U& result) {
    size_t count = 0;
    std::string currentLine;

    result.erase();

    while (std::getline(stream, currentLine)) {
        count++;
        if (!smack::util::strings::ends_with(currentLine, "\\")) {
            result.append(currentLine);
            break;
        }
        result = result.append(
            currentLine.substr(0, currentLine.size() - 1));
    }

    return count != 0;
}
}

namespace smack::util::properties
{
    PropertyMap loadProperties(std::ifstream& infile)
    {
        const auto COMMENT_CHAR = '#';
        const auto EQUALS_CHAR = '=';

        PropertyMap result;

        std::string line;

        if (!infile.is_open())
            return result;

        size_t lineCount = 0;
        while (nextLine(infile, line)) {
            lineCount++;

            if (line.empty())
                continue;
            std::string_view buffer{ line };
            while (std::isspace(buffer[0]))
                buffer = buffer.substr(1);
            if (buffer.empty())
                continue;
            if (buffer[0] == COMMENT_CHAR)
                continue;
            if (buffer[0] == EQUALS_CHAR)
                throw std::runtime_error("Empty key@" + std::to_string(lineCount));

            auto equalsPosition = buffer.find(EQUALS_CHAR);
            if (equalsPosition == string::npos)
                throw std::runtime_error("Missing '='@" + std::to_string(lineCount));

            auto key = buffer.substr(0, equalsPosition);
            auto val = buffer.substr(equalsPosition + 1);

            result[string{ key }] = string{ val };
        }

        return result;
    }

    PropertyMap loadProperties(string filename)
    {
        std::ifstream infile(filename);

        return loadProperties(infile);
    }
} // namespace smack::util::properties
