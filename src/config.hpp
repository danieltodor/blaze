#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>

const std::string control_char = "`";

struct Global
{
    std::string padding = " ";
    int execution_time_precision = 1;
    int execution_time_display_from = 0;
    int execution_time_display_fractional_until = 10;
};

struct Prompt
{
    std::string string = " ";
    std::string foreground = "";
};

struct Connector
{
    std::string character = " ";
    std::string foreground = "";
};

struct Segment
{
    std::string name = "";
    std::string execute = "";
    int level = 1;
    int position = 1;
    std::string align = "left";
    std::string padding = control_char;
    std::string inner_prefix = "";
    std::string inner_suffix = "";
    std::string outer_prefix = "";
    std::string outer_suffix = "";
    std::string background = "";
    std::string foreground = "";
    bool bold = false;
    bool dim = false;
    bool italic = false;
    bool underline = false;
};

class Config
{
public:
    Global global;
    std::vector<Segment> segments;
    Connector connector;
    Prompt prompt;
    Config();
    void parse_config();
    void sort_segments();
    void set_default_config();
    Segment get_previous_segment(std::size_t current_index);
    Segment get_next_segment(std::size_t current_index);
};

#endif
