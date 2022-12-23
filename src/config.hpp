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

struct prompt
{
    std::string string = "";
    int foreground = 0;
};

struct connector
{
    std::string character = "";
    int foreground = 0;
};

class config
{
public:
    std::vector<segment> segments;
    prompt ps1;
    connector conn;
    config();
    void parse_config();
    void sort_segments();
    void set_default_config();
    segment get_previous_segment(std::size_t current_index);
    segment get_next_segment(std::size_t current_index);
};

#endif
