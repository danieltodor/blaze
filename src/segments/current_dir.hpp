#ifndef BLAZE_SEGMENTS_CURRENT_DIR_HPP_INCLUDED
#define BLAZE_SEGMENTS_CURRENT_DIR_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

std::string current_dir(Context context)
{
    std::string result = "";
    std::string &PWD = context.PWD;
    std::string HOME = std::getenv("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += PWD;
    return result;
}

#endif
