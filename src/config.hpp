#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>

#define SEGMENT_COUNT 4

struct segment
{
    std::string name = "";
    std::string side = "";
    std::string start_char = "";
    std::string end_char = "";
    int background = 0;
    int foreground = 0;
};

struct sprompt
{
    std::string fixed = "";
    int background = 0;
    int foreground = 0;
};

class config
{
public:
    segment segments[SEGMENT_COUNT];
    segment current_sgm;
    segment prev_sgm;
    segment next_sgm;
    sprompt prompt;
    config();
    void parse_config();
};

#endif
