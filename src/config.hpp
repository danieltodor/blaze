#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>

#include "external/toml/toml.hpp"

const std::string control_char = "`";

struct Global
{
    bool new_line = false;
    std::string padding = " ";
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
    std::string content = "";
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

struct CurrentDirectory
{
    bool basename_only = false;
};

struct ExecutionTime
{
    int precision = 1;
    int display_from = 0;
    int display_fractional_until = 10;
};

struct GitBranch
{
    std::vector<std::string> ignore;
};

struct GitStatus
{
    std::string conflicted = "=";
    std::string ahead = "⇡";
    std::string behind = "⇣";
    std::string diverged = "⇕";
    std::string up_to_date = "";
    std::string untracked = "?";
    std::string stashed = "$";
    std::string modified = "!";
    std::string staged = "+";
    std::string renamed = "»";
    std::string deleted = "✘";
};

class Config
{
public:
    Global global;
    Prompt prompt;
    Connector connector;
    std::vector<Segment> segments;
    CurrentDirectory current_directory;
    ExecutionTime execution_time;
    GitBranch git_branch;
    GitStatus git_status;
    Config();
    toml::value load_config();
    void parse_config(toml::value data);
    void sort_segments();
    void set_default_config();
    Segment get_previous_segment_in_group(std::size_t current_index);
    Segment get_next_segment_in_group(std::size_t current_index);
};

#endif
