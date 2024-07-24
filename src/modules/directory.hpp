#ifndef BLAZE_MODULES_DIRECTORY_HPP_INCLUDED
#define BLAZE_MODULES_DIRECTORY_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Current working directory
std::string directory(const Context &context)
{
    std::string result = "";
    const Config &config = context.config;
    std::string PWD = context.PWD;
    std::string HOME = get_env("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += PWD;
    if (config.directory.basename_only && result != "/")
    {
        const std::size_t index = find_nth_occurrence(result, "/", 1, true);
        if (index != std::string::npos)
        {
            result.replace(0, index + 1, "");
        }
    }
    else if (config.directory.length > 0)
    {
        const std::size_t index = find_nth_occurrence(result, "/", config.directory.length, true);
        if (index != std::string::npos)
        {
            result.replace(0, index, "...");
        }
    }
    return result;
}

#endif
