#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "config.hpp"

toml::value Config::load_config()
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
    return data;
}

template<typename T, typename... Keys>
void set_value(toml::value data, T &target, Keys&&... keys)
{
    try
    {
        target = toml::find<T>(data, keys...);
    }
    catch (const std::out_of_range &err) {}
}

void Config::parse_config(toml::value data)
{
    this->set_default_config();
    if (data.is_uninitialized())
    {
        return;
    }

    set_value(data, this->global.padding, "global", "padding");
    set_value(data, this->global.execution_time_precision, "global", "execution_time_precision");
    set_value(data, this->global.execution_time_display_from, "global", "execution_time_display_from");
    set_value(data, this->global.execution_time_display_fractional_until, "global", "execution_time_display_fractional_until");

    set_value(data, this->prompt.string, "prompt", "string");
    set_value(data, this->prompt.foreground, "prompt", "foreground");

    set_value(data, this->connector.character, "connector", "character");
    set_value(data, this->connector.foreground, "connector", "foreground");

    try
    {
        toml::array &segment_array = toml::find(data, "segment").as_array();
        this->segments.clear();
        try
        {
            for (int i = 0;; i++)
            {
                toml::value &segment_data = segment_array.at(i);
                Segment current;
                set_value(segment_data, current.name, "name");
                set_value(segment_data, current.execute, "execute");
                set_value(segment_data, current.level, "level");
                set_value(segment_data, current.position, "position");
                set_value(segment_data, current.align, "align");
                set_value(segment_data, current.padding, "padding");
                set_value(segment_data, current.inner_prefix, "inner_prefix");
                set_value(segment_data, current.inner_suffix, "inner_suffix");
                set_value(segment_data, current.outer_prefix, "outer_prefix");
                set_value(segment_data, current.outer_suffix, "outer_suffix");
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

void Config::sort_segments()
{
    std::unordered_map<std::string, int> sides;
    sides["left"] = 1;
    sides["right"] = 2;
    auto compare = [&sides](Segment a, Segment b)
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

Config::Config()
{
    this->parse_config(this->load_config());
    this->sort_segments();
}

Segment Config::get_previous_segment(std::size_t current_index)
{
    Segment previous;
    if (current_index == 0)
    {
        return previous;
    }
    Segment tmp = this->segments[current_index - 1];
    if (tmp.level == this->segments[current_index].level && tmp.align == this->segments[current_index].align)
    {
        previous = tmp;
    }
    return previous;
}

Segment Config::get_next_segment(std::size_t current_index)
{
    Segment next;
    if (current_index == this->segments.size() - 1)
    {
        return next;
    }
    Segment tmp = this->segments[current_index + 1];
    if (tmp.level == this->segments[current_index].level && tmp.align == this->segments[current_index].align)
    {
        next = tmp;
    }
    return next;
}

void Config::set_default_config()
{
    this->global.padding = "";
    this->global.execution_time_display_from = 2;

    Prompt prompt;
    prompt.string = "\n❯ ";
    this->prompt = prompt;

    Segment current_dir;
    current_dir.name = "current_dir";
    current_dir.level = 1;
    current_dir.position = 1;
    current_dir.align = "left";
    current_dir.foreground = "blue";
    current_dir.bold = true;
    this->segments.push_back(current_dir);

    Segment execution_time;
    execution_time.name = "execution_time";
    execution_time.level = 1;
    execution_time.position = 2;
    execution_time.align = "left";
    execution_time.outer_prefix = " ";
    execution_time.foreground = "yellow";
    this->segments.push_back(execution_time);
}
