#ifndef BLAZE_SEGMENTS_CURRENT_DIRECTORY_HPP_INCLUDED
#define BLAZE_SEGMENTS_CURRENT_DIRECTORY_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string current_directory(Context context)
{
    std::string result = "";
    std::string &PWD = context.PWD;
    Config &config = context.config;
    std::string HOME = std::getenv("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += PWD;
    if (result != "/" and config.current_directory.basename_only)
    {
        regex_replace(result, {"^.*\\/"}, "");
    }
    return result;
}

#endif
