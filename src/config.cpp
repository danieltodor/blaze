#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "config.hpp"
#include "external/toml.hpp"

template<typename T, typename... Keys>
void set_value(toml::value data, T &target, Keys&&... keys)
{
    try
    {
        target = toml::find<T>(data, keys...);
    }
    catch (const std::out_of_range &err) {}
}

void config::parse_config()
{
    std::string HOME = std::getenv("HOME");
    std::string paths[] = {
        HOME + "/.config/blaze.toml",
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
        catch (const std::runtime_error &err) {}
    }
    this->set_default_config();
    if (data.is_uninitialized())
    {
        return;
    }

    set_value(data, this->glob.padding, "global", "padding");
    set_value(data, this->glob.execution_time_precision, "global", "execution_time_precision");
    set_value(data, this->glob.execution_time_display_from, "global", "execution_time_display_from");
    set_value(data, this->glob.execution_time_display_fractional_until, "global", "execution_time_display_fractional_until");

    set_value(data, this->ps1.string, "prompt", "string");
    set_value(data, this->ps1.foreground, "prompt", "foreground");

    set_value(data, this->conn.character, "connector", "character");
    set_value(data, this->conn.foreground, "connector", "foreground");

    try
    {
        toml::array &segment_array = toml::find(data, "segment").as_array();
        this->segments.clear();
        try
        {
            for (int i = 0;; i++)
            {
                toml::value &segment_data = segment_array.at(i);
                segment current;
                set_value(segment_data, current.name, "name");
                set_value(segment_data, current.execute, "execute");
                set_value(segment_data, current.level, "level");
                set_value(segment_data, current.position, "position");
                set_value(segment_data, current.align, "align");
                set_value(segment_data, current.prefix, "prefix");
                set_value(segment_data, current.suffix, "suffix");
                set_value(segment_data, current.background, "background");
                set_value(segment_data, current.foreground, "foreground");
                set_value(segment_data, current.bold, "bold");
                set_value(segment_data, current.dim, "dim");
                set_value(segment_data, current.italic, "italic");
                set_value(segment_data, current.underline, "underline");
                this->segments.push_back(current);
            }
        }
        catch (const std::out_of_range &err) {}
    }
    catch (const std::out_of_range &err) {}
}

void config::sort_segments()
{
    std::unordered_map<std::string, int> sides;
    sides["left"] = 1;
    sides["right"] = 2;
    auto compare = [&sides](segment a, segment b)
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
    std::sort(this->segments.begin(), this->segments.end(), compare);
}

config::config()
{
    this->parse_config();
    this->sort_segments();
}

segment config::get_previous_segment(std::size_t current_index)
{
    segment previous;
    if (current_index == 0)
    {
        return previous;
    }
    segment tmp = this->segments[current_index - 1];
    if (tmp.level == this->segments[current_index].level && tmp.align == this->segments[current_index].align)
    {
        previous = tmp;
    }
    return previous;
}

segment config::get_next_segment(std::size_t current_index)
{
    segment next;
    if (current_index == this->segments.size() - 1)
    {
        return next;
    }
    segment tmp = this->segments[current_index + 1];
    if (tmp.level == this->segments[current_index].level && tmp.align == this->segments[current_index].align)
    {
        next = tmp;
    }
    return next;
}

void config::set_default_config()
{
    this->glob.padding = "";
    this->glob.execution_time_display_from = 2;

    prompt ps1;
    ps1.string = "\n❯ ";
    this->ps1 = ps1;

    segment current_dir;
    current_dir.name = "current_dir";
    current_dir.level = 1;
    current_dir.position = 1;
    current_dir.align = "left";
    current_dir.foreground = "blue";
    current_dir.bold = true;
    this->segments.push_back(current_dir);

    segment execution_time;
    execution_time.name = "execution_time";
    execution_time.level = 1;
    execution_time.position = 2;
    execution_time.align = "left";
    execution_time.prefix = " ";
    execution_time.foreground = "yellow";
    this->segments.push_back(execution_time);
}
