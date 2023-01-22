#include "utils.hpp"

void regex_replace(std::string &string, const std::string patterns[], const std::string &replacement)
{
    std::string pattern;
    for (int i = 0;; i++)
    {
        try
        {
            pattern = patterns[i];
        }
        catch (const std::exception &err)
        {
            break;
        }
        string = std::regex_replace(string, std::regex(pattern), replacement);
    }
}

void strip(std::string &string)
{
    const std::string patterns[] = {
        "^\\s*",
        "\\s*$"
    };
    regex_replace(string, patterns, "");
}
