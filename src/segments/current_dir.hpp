#ifndef BLAZE_CURRENT_DIR_HPP_INCLUDED
#define BLAZE_CURRENT_DIR_HPP_INCLUDED

#include <string>

std::string current_dir()
{
    std::string result = "";
    std::string PWD = std::getenv("PWD");
    std::string HOME = std::getenv("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += ' ' + PWD + ' ';
    return result;
}

#endif
