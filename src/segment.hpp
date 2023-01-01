#ifndef BLAZE_SEGMENT_HPP_INCLUDED
#define BLAZE_SEGMENT_HPP_INCLUDED

#include <string>

#include "config.hpp"

std::string call_segment(std::string name, config conf, double start_time, double finish_time);
std::string execute_segment(std::string command);

#endif
