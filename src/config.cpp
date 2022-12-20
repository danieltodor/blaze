#include <iostream>

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

config::config()
{
    this->parse_config();
}

segment config::get_previous_segment()
{
    segment prev;
    for (std::size_t i = 0; i < this->segments.size(); i++)
    {
        segment current = this->segments[i];
        if (current.level == this->current_sgm.level && current.side == this->current_sgm.side && current.position == this->current_sgm.position - 1)
        {
            prev = this->segments[i];
        }
    }
    return prev;
}

segment config::get_next_segment()
{
    segment next;
    for (std::size_t i = 0; i < this->segments.size(); i++)
    {
        segment current = this->segments[i];
        if (current.level == this->current_sgm.level && current.side == this->current_sgm.side && current.position == this->current_sgm.position + 1)
        {
            next = this->segments[i];
        }
    }
    return next;
}
