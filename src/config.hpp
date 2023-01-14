#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>

struct global
{
    std::string padding = " ";
    int execution_time_precision = 1;
    int execution_time_display_from = 0;
    int execution_time_display_fractional_until = 10;
};

struct prompt
{
    std::string string = " ";
    std::string foreground = "";
};

struct connector
{
    std::string character = " ";
    std::string foreground = "";
};

struct segment
{
    std::string name = "";
    std::string execute = "";
    int level = 1;
    int position = 1;
    std::string align = "left";
    std::string inner_prefix = "";
    std::string outer_prefix = "";
    std::string inner_suffix = "";
    std::string outer_suffix = "";
    std::string background = "";
    std::string foreground = "";
    bool bold = false;
    bool dim = false;
    bool italic = false;
    bool underline = false;
};

class config
{
public:
    global glob;
    std::vector<segment> segments;
    connector conn;
    prompt ps1;
    config();
    void parse_config();
    void sort_segments();
    void set_default_config();
    segment get_previous_segment(std::size_t current_index);
    segment get_next_segment(std::size_t current_index);
};

#endif
