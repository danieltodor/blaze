#ifndef BLAZE_CONTEXT_HPP_INCLUDED
#define BLAZE_CONTEXT_HPP_INCLUDED

#include <string>

#include "config.hpp"

struct Context
{
    Config config;
    std::string shell;
    double start_time;
    double finish_time;
    std::string PWD;
    bool git_repository_detected;
};

#endif
