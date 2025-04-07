#pragma once

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
    bool git_repository_detached;
    std::tm time_structure;
};

// Create context
Context get_context(int argc, char *argv[]);
