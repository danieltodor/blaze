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

#endif
