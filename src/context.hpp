#ifndef BLAZE_CONTEXT_HPP_INCLUDED
#define BLAZE_CONTEXT_HPP_INCLUDED

#include <string>
#include <time.h>

#include "config.hpp"

struct Context
{
    Config config;
    std::string shell;
    double start_time;
    double finish_time;
    std::string default_background;
    std::string PWD;
    bool git_repository_detected;
    std::tm time_structure;
};

#endif
