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
    std::string background = "";
    bool dim = false;
};

struct Module
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
    std::string foreground = "";
    std::string background = "";
    bool bold = false;
    bool dim = false;
    bool italic = false;
    bool underline = false;
};

struct Directory
{
    bool basename_only = false;
};

struct ExecutionTime
{
    int precision = 1;
    float display_from = 0;
    float display_fractional_until = 10;
};

struct GitBranch
{
    std::vector<std::string> ignore;
};

struct GitStatus
{
    std::string clean = "";
    std::string conflicted = "=";
    std::string ahead = "⇡";
    std::string behind = "⇣";
    std::string diverged = "⇕";
    std::string untracked = "?";
    std::string stashed = "$";
    std::string modified = "!";
    std::string staged = "+";
    std::string renamed = "»";
    std::string deleted = "✘";
};

struct Date
{
    std::string format = "%x";
};

struct Time
{
    std::string format = "%X";
};

struct Config
{
    Global global;
    Prompt prompt;
    Connector connector;
    bool connector_displayed;
    std::vector<Module> modules;
    Directory directory;
    ExecutionTime execution_time;
    GitBranch git_branch;
    GitStatus git_status;
    Date date;
    Time time;
};

Module *get_previous_module_in_group(std::vector<Module> &modules, const std::size_t current_index);
Module *get_next_module_in_group(std::vector<Module> &modules, const std::size_t current_index);
std::vector<Module *> modules_on_level(std::vector<Module> &modules, int level);
bool content_on_right(std::vector<Module> &modules, int level);
Config get_config();

#endif
