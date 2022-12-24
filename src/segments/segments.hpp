#ifndef BLAZE_SEGMENTS_HPP_INCLUDED
#define BLAZE_SEGMENTS_HPP_INCLUDED

#include <string>

#include "current_dir.hpp"
#include "execution_time.hpp"

std::string execute_segment(std::string name, double start_time, double finish_time)
{
    std::string result = "";
    if (name == "current_dir"){result = current_dir();}
    else if (name == "execution_time"){result = execution_time(start_time, finish_time);}
    return result;
}

std::string call_segment(std::string command)
{
    std::string result = "";
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

#endif
