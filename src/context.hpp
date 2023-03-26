#ifndef BLAZE_CONTEXT_HPP_INCLUDED
#define BLAZE_CONTEXT_HPP_INCLUDED

#include <string>
#include <time.h>

#include "src/config.hpp"

struct Context
{
    Config config;
    std::string shell;
    bool prompt;
    bool rprompt;
    double start_time;
    double finish_time;
    std::string exit_status;
    std::string default_background;
    std::string PWD;
    bool git_repository_detected;
    std::tm time_structure;
};

#endif
