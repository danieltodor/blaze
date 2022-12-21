#include <iostream>
#include <algorithm>

#include "config.hpp"
#include "toml.hpp"
#include "colors.hpp"

void config::parse_config()
{
    std::string HOME = std::getenv("HOME");
    std::string paths[] = {
        HOME + "/.config/blaze.toml",
        "/etc/blaze.toml"
    };
    toml::table tbl;
    try
    {
        for (std::size_t i = 0; i < std::size(paths); i++)
        {
            tbl = toml::parse_file(paths[i]);
            break;
        }
    }
    catch (const toml::parse_error &err)
    {
        std::cout << "Failed to parse config: " << err;
    }
    std::unordered_map<std::string, int> colors = color_map();
    int i = 0;
    while (true)
    {
        if (!tbl["segment"][i])
        {
            break;
        }
        segment current;
        current.position = tbl["segment"][i]["position"].value_or(1);
        current.level = tbl["segment"][i]["level"].value_or(1);
        current.name = tbl["segment"][i]["name"].value_or("");
        current.side = tbl["segment"][i]["side"].value_or("left");
        current.start_char = tbl["segment"][i]["start_char"].value_or("");
        current.end_char = tbl["segment"][i]["end_char"].value_or("");
        current.background = colors[tbl["segment"][i]["background"].value_or("none")];
        current.foreground = colors[tbl["segment"][i]["foreground"].value_or("none")];
        this->segments.push_back(current);
        i++;
    }
    this->prompt.fixed = tbl["prompt"]["fixed"].value_or(" ");
    this->prompt.background = colors[tbl["prompt"]["background"].value_or("none")];
    this->prompt.foreground = colors[tbl["prompt"]["foreground"].value_or("none")];
}

void config::sort_segments()
{
    std::unordered_map<std::string, int> sides;
    sides["left"] = 1;
    sides["right"] = 2;
    auto compare = [&sides](segment a, segment b)
    {
        std::string a_value, b_value = "";
        a_value += a.level;
        a_value += sides[a.side];
        a_value += a.position;
        b_value += b.level;
        b_value += sides[b.side];
        b_value += b.position;
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
    segment tmp = this->segments.at(current_index - 1);
    if (tmp.level == this->current_sgm.level && tmp.side == this->current_sgm.side)
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
    segment tmp = this->segments.at(current_index + 1);
    if (tmp.level == this->current_sgm.level && tmp.side == this->current_sgm.side)
    {
        next = tmp;
    }
    return next;
}