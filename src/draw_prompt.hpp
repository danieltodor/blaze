#ifndef BLAZE_DRAW_PROMPT_HPP_INCLUDED
#define BLAZE_DRAW_PROMPT_HPP_INCLUDED

#include <string>

// Settings
#define SEGMENT_CONNECTOR_CHAR '─'
#define EXECUTION_TIME_PRECISION 1
#define EXECUTION_TIME_DISPLAY_FROM 1
#define EXECUTION_TIME_DISPLAY_FRACTIONAL_UNTIL 10

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

std::string current_dir();
std::string execution_time(double start_time, double finish_time);
std::string prompt();
void draw_prompt(double start_time, double finish_time);

#endif
