#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>

struct segment
{
    int position = 0;
    int level = 0;
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
    std::vector<segment> segments;
    segment current_sgm;
    segment prev_sgm;
    segment next_sgm;
    sprompt prompt;
    config();
    void parse_config();
    segment get_previous_segment();
    segment get_next_segment();
};

#endif
