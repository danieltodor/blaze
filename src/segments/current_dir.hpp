#ifndef BLAZE_SEGMENTS_CURRENT_DIR_HPP_INCLUDED
#define BLAZE_SEGMENTS_CURRENT_DIR_HPP_INCLUDED

#include <string>

#include "src/segment.hpp"

std::string current_dir(Context context)
{
    (void)context;
    std::string result = "";
    std::string PWD = std::getenv("PWD");
    std::string HOME = std::getenv("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += PWD;
    return result;
}

#endif
