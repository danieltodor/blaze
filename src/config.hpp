#ifndef BLAZE_CONFIG_HPP_INCLUDED
#define BLAZE_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>

#include "external/toml/toml.hpp"

// Character that is unlikely to be used. Easy to check if the user changed the default.
// TODO: Can i use \0 for this purpose?
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
    std::string error_foreground = "";
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
    bool count = true;
    std::string spacing = " ";
    std::string clean = "✓";
    std::string ahead = "↑";
    std::string behind = "↓";
    std::string stashed = "$";
    std::string untracked = "?";
    std::string conflicted = "=";
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

struct ExitStatus
{
    bool non_zero_only = false;
};

struct Config
{
    Global global;
    Prompt prompt;
    Connector connector;
    std::vector<Module> modules;
    Directory directory;
    ExecutionTime execution_time;
    GitBranch git_branch;
    GitStatus git_status;
    Date date;
    Time time;
    ExitStatus exit_status;
};

// Read the user`s config file, or set default values if there is no such file
Config get_config();

// Previous module in the current group. Same level, same side
Module *get_previous_module_in_group(std::vector<Module> &modules, const std::size_t index);

// Next module in the current group. Same level, same side
Module *get_next_module_in_group(std::vector<Module> &modules, const std::size_t index);

// Modules on the current level
std::vector<Module *> get_modules_on_level(std::vector<Module> &modules, const int level);

// At least one module on the right has content set
bool contains_content_on_right(std::vector<Module> &modules, const int level);

// Module is a separator
bool is_separator(const Module &module);

#endif
