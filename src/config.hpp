#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>

struct segment
{
    std::string name = "";
    std::string execute = "";
    int level = 0;
    int position = 0;
    std::string align = "";
    std::string prefix = "";
    std::string suffix = "";
    std::string background = "";
    std::string foreground = "";
    bool bold = false;
    bool dim = false;
    bool italic = false;
    bool underline = false;
};

struct prompt
{
    std::string string = "";
    std::string foreground = "";
};

struct connector
{
    std::string character = "";
    std::string foreground = "";
};

class config
{
public:
    std::vector<segment> segments;
    prompt ps1;
    connector conn;
    std::string padding;
    int execution_time_precision;
    int execution_time_display_from;
    int execution_time_display_fractional_until;
    config();
    void parse_config();
    void sort_segments();
    void set_default_config();
    segment get_previous_segment(std::size_t current_index);
    segment get_next_segment(std::size_t current_index);
};

#endif
