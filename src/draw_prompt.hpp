#ifndef BLAZE_DRAW_PROMPT_HPP_INCLUDED
#define BLAZE_DRAW_PROMPT_HPP_INCLUDED

#include <string>

#include "config.hpp"

// Settings
#define SEGMENT_CONNECTOR_CHAR '─'
#define EXECUTION_TIME_PRECISION 1
#define EXECUTION_TIME_DISPLAY_FROM 1
#define EXECUTION_TIME_DISPLAY_FRACTIONAL_UNTIL 10

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

void draw_prompt(std::string shell, double start_time, double finish_time);

class artist
{
public:
    artist(std::string shell, config conf);
    std::string shell;
    config conf;
    std::string pre();
    std::string post();
    std::string current_dir();
    std::string execution_time(double start_time, double finish_time);
    std::string prompt();
};

#endif
