#ifndef BLAZE_MODULES_DIRECTORY_HPP_INCLUDED
#define BLAZE_MODULES_DIRECTORY_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

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
    if (result != "/" and config.directory.basename_only)
    {
        regex_replace(result, {"^.*\\/"}, "");
    }
    return result;
}

#endif
