#ifndef BLAZE_CONTEXT_HPP_INCLUDED
#define BLAZE_CONTEXT_HPP_INCLUDED

#include <string>
#include <time.h>

#include "src/arg.hpp"
#include "src/config.hpp"

struct Context
{
    Args args;
    Config config;
    std::string HOME;
    std::string PWD;
    bool git_repository_detected;
    std::tm time_structure;
};

#endif
