#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "config.hpp"
#include "util.hpp"

toml::value Config::load_config()
{
    const std::string paths[] = {
        get_env("BLAZE_CONFIG"),
        get_env("HOME") + "/.config/blaze.toml",
        "/etc/blaze.toml"
    };
    toml::value data;
    for (std::size_t i = 0; i < std::size(paths); i++)
    {
        try
        {
            data = toml::parse(paths[i]);
            break;
        }
        catch (const std::runtime_error &err)
        {
        }
    }
    return data;
}

template<typename T, typename... Keys>
void set_value(const toml::value &data, T &target, Keys&&... keys)
{
    try
    {
        target = toml::find<T>(data, keys...);
    }
    catch (const std::out_of_range &err)
    {
    }
}

void Config::parse_config(toml::value &data)
{
    this->set_default_config();
    if (data.is_uninitialized())
    {
        return;
    }

    set_value(data, this->global.new_line, "global", "new_line");
    set_value(data, this->global.padding, "global", "padding");

    set_value(data, this->prompt.string, "prompt", "string");
    set_value(data, this->prompt.foreground, "prompt", "foreground");

    set_value(data, this->connector.character, "connector", "character");
    set_value(data, this->connector.foreground, "connector", "foreground");
    set_value(data, this->connector.background, "connector", "background");
    set_value(data, this->connector.dim, "connector", "dim");

    try
    {
        const toml::array &module_array = toml::find(data, "module").as_array();
        this->modules.clear();
        try
        {
            for (int i = 0;; i++)
            {
                const toml::value &module_data = module_array.at(i);
                Module current;
                set_value(module_data, current.name, "name");
                set_value(module_data, current.execute, "execute");
                set_value(module_data, current.level, "level");
                set_value(module_data, current.position, "position");
                set_value(module_data, current.align, "align");
                set_value(module_data, current.padding, "padding");
                set_value(module_data, current.inner_prefix, "inner_prefix");
                set_value(module_data, current.inner_suffix, "inner_suffix");
                set_value(module_data, current.outer_prefix, "outer_prefix");
                set_value(module_data, current.outer_suffix, "outer_suffix");
                set_value(module_data, current.foreground, "foreground");
                set_value(module_data, current.background, "background");
                set_value(module_data, current.bold, "bold");
                set_value(module_data, current.dim, "dim");
                set_value(module_data, current.italic, "italic");
                set_value(module_data, current.underline, "underline");
                this->modules.push_back(current);
            }
        }
        catch (const std::out_of_range &err)
        {
        }
    }
    catch (const std::out_of_range &err)
    {
    }

    set_value(data, this->directory.basename_only, "directory", "basename_only");

    set_value(data, this->execution_time.precision, "execution_time", "precision");
    set_value(data, this->execution_time.display_from, "execution_time", "display_from");
    set_value(data, this->execution_time.display_fractional_until, "execution_time", "display_fractional_until");

    set_value(data, this->git_branch.ignore, "git_branch", "ignore");

    set_value(data, this->git_status.clean, "git_status", "clean");
    set_value(data, this->git_status.conflicted, "git_status", "conflicted");
    set_value(data, this->git_status.ahead, "git_status", "ahead");
    set_value(data, this->git_status.behind, "git_status", "behind");
    set_value(data, this->git_status.diverged, "git_status", "diverged");
    set_value(data, this->git_status.untracked, "git_status", "untracked");
    set_value(data, this->git_status.stashed, "git_status", "stashed");
    set_value(data, this->git_status.modified, "git_status", "modified");
    set_value(data, this->git_status.staged, "git_status", "staged");
    set_value(data, this->git_status.renamed, "git_status", "renamed");
    set_value(data, this->git_status.deleted, "git_status", "deleted");

    set_value(data, this->date.format, "date", "format");

    set_value(data, this->time.format, "time", "format");
}

void Config::sort_modules()
{
    std::unordered_map<std::string, int> sides;
    sides["left"] = 1;
    sides["right"] = 2;
    auto compare = [&sides](const Module &a, const Module &b)
    {
        std::string a_value = "", b_value = "";
        a_value += std::to_string(a.level);
        a_value += std::to_string(sides[a.align]);
        a_value += std::to_string(a.position);
        b_value += std::to_string(b.level);
        b_value += std::to_string(sides[b.align]);
        b_value += std::to_string(b.position);
        return a_value < b_value;
    };
    std::sort(this->modules.begin(), this->modules.end(), compare);
}

Config::Config()
{
    toml::value config = this->load_config();
    this->parse_config(config);
    this->sort_modules();
}

Module *Config::get_previous_module_in_group(const std::size_t current_index)
{
    Module *previous = NULL;
    if (current_index == 0)
    {
        return previous;
    }
    Module &tmp = this->modules[current_index - 1];
    if (tmp.level == this->modules[current_index].level && tmp.align == this->modules[current_index].align)
    {
        previous = &tmp;
    }
    return previous;
}

Module *Config::get_next_module_in_group(const std::size_t current_index)
{
    Module *next = NULL;
    if (current_index == this->modules.size() - 1)
    {
        return next;
    }
    Module &tmp = this->modules[current_index + 1];
    if (tmp.level == this->modules[current_index].level && tmp.align == this->modules[current_index].align)
    {
        next = &tmp;
    }
    return next;
}

std::vector<Module *> Config::modules_on_level(int level)
{
    std::vector<Module *> result;
    for (Module &module : this->modules)
    {
        if (module.level < level)
        {
            continue;
        }
        else if (module.level == level)
        {
            result.push_back(&module);
        }
        else if (module.level > level)
        {
            break;
        }
    }
    return result;
}

bool Config::content_on_right(int level)
{
    std::vector<Module *> modules = this->modules_on_level(level);
    for (const Module *module : modules)
    {
        if (module->align == "right" && !module->content.empty())
        {
            return true;
        }
    }
    return false;
}

void Config::set_default_config()
{
    this->global.padding = "";
    this->execution_time.display_from = 2;

    Prompt prompt;
    prompt.string = "\n❯ ";
    this->prompt = prompt;

    Module directory;
    directory.name = "directory";
    directory.level = 1;
    directory.position = 1;
    directory.align = "left";
    directory.foreground = "blue";
    directory.bold = true;
    this->modules.push_back(directory);

    Module execution_time;
    execution_time.name = "execution_time";
    execution_time.level = 1;
    execution_time.position = 2;
    execution_time.align = "left";
    execution_time.outer_prefix = " ";
    execution_time.foreground = "yellow";
    this->modules.push_back(execution_time);
}
