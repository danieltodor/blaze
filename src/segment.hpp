#ifndef BLAZE_SEGMENT_HPP_INCLUDED
#define BLAZE_SEGMENT_HPP_INCLUDED

#include <string>

#include "config.hpp"

struct Context
{
    Config config;
    double start_time;
    double finish_time;
};
std::string call_segment(std::string name, Context context);
std::string execute_segment(std::string command);

#endif
