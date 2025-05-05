#pragma once

#include "types.hpp"

// Character that is unlikely to be used. Easy to check if the user changed the default.
#define CONTROL_CHAR "`"
// Alignments
#define LEFT_ALIGNMENT "left"
#define RIGHT_ALIGNMENT "right"
#define RP_ALIGNMENT "right_prompt"

struct Global
{
    std::string padding = "";
};

struct Prompt
{
    std::string string = " ❯ ";
    std::string foreground = "";
    std::string error_foreground = "";
    bool new_line = false;
    std::string separator = "";
    bool transient = false;
    bool bottom = false;
};

struct Connector
{
    std::string character = " ";
    std::string foreground = "";
    std::string background = "";
    bool dim = false;
};

struct Directory
{
    int limit = 10;
    bool from_repository = true;
};

struct Duration
{
    float threshold = 2;
    int precision = 1;
    float fractional_until = 10;
};

struct GitBranch
{
    bool show_when_detached = false;
    StringVector ignore;
};

struct GitCommit
{
    bool show_when_attached = false;
    int length = 7;
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

struct Status
{
    bool show_zero = false;
};

struct Module
{
    std::string name = "";
    std::string execute = "";
    std::string content = "";
    int level = 1;
    int position = 1;
    std::string align = LEFT_ALIGNMENT;
    std::string padding = CONTROL_CHAR;
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
using ModuleVector = std::vector<Module>;
using ModulePtrVector = std::vector<Module *>;

struct Config
{
    Global global;
    Prompt prompt;
    Connector connector;
    Directory directory;
    Duration duration;
    GitBranch git_branch;
    GitCommit git_commit;
    GitStatus git_status;
    Date date;
    Time time;
    Status status;
    ModuleVector modules;
};

// Read the user's config file, or set default values if there is no such file
Config get_config();

// Previous module in the current group. Same level, same side
Module *get_previous_module_in_group(ModuleVector &modules, const std::size_t index);

// Next module in the current group. Same level, same side
Module *get_next_module_in_group(ModuleVector &modules, const std::size_t index);

// Modules on the current level
ModulePtrVector get_modules_on_level(ModuleVector &modules, const int level);

// At least one module on the right has content set
bool contains_content_on_right(ModuleVector &modules, const int level);

// Vertical size of the displayed config
int vertical_size(const Config &config);
