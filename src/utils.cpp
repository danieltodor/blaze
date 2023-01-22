#include <regex>

#include "utils.hpp"

void regex_replace(std::string &string, const std::vector<std::string> patterns, const std::string &replacement)
{
    for (const std::string &pattern : patterns)
    {
        string = std::regex_replace(string, std::regex(pattern), replacement);
    }
}

bool regex_search(std::string &string, const std::vector<std::string> patterns)
{
    bool result = false;
    for (const std::string &pattern : patterns)
    {
        if (std::regex_search(string, std::regex(pattern)))
        {
            result = true;
            break;
        }
    }
    return result;
}

void strip(std::string &string)
{
    regex_replace(
        string,
        {
            "^\\s*",
            "\\s*$"
        },
        "");
}
