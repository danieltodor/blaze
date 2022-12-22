#ifndef BLAZE_DRAW_PROMPT_HPP_INCLUDED
#define BLAZE_DRAW_PROMPT_HPP_INCLUDED

#include <string>

#include "config.hpp"

void draw_prompt(std::string shell, double start_time, double finish_time);

class artist
{
public:
    artist(std::string shell, config conf);
    std::string shell;
    config conf;
    std::string pre();
    std::string post();
    std::string prompt();
};

#endif
