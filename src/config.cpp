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
    bool config_parsed = false;
    for (std::size_t i = 0; i < std::size(paths); i++)
    {
        try
        {
            tbl = toml::parse_file(paths[i]);
            config_parsed = true;
            break;
        }
        catch (const toml::parse_error &err) {}
    }
    if (!config_parsed)
    {
        this->set_default_config();
        return;
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
    this->ps1.string = tbl["prompt"]["string"].value_or(" ");
    this->ps1.foreground = colors[tbl["prompt"]["foreground"].value_or("none")];
    this->conn.character = tbl["connector"]["character"].value_or("─");
    this->conn.foreground = colors[tbl["connector"]["foreground"].value_or("none")];
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
    segment tmp = this->segments[current_index - 1];
    if (tmp.level == this->segments[current_index].level && tmp.side == this->segments[current_index].side)
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
    if (tmp.level == this->segments[current_index].level && tmp.side == this->segments[current_index].side)
    {
        next = tmp;
    }
    return next;
}

void config::set_default_config()
{
    segment current_dir;
    current_dir.name = "current_dir";
    current_dir.level = 1;
    current_dir.position = 1;
    current_dir.side = "left";
    current_dir.background = BLUE;
    current_dir.foreground = BLACK;
    current_dir.end_char = "";
    this->segments.push_back(current_dir);

    segment execution_time;
    execution_time.name = "execution_time";
    execution_time.level = 1;
    execution_time.position = 2;
    execution_time.side = "left";
    execution_time.foreground = YELLOW;
    this->segments.push_back(execution_time);

    prompt ps1;
    ps1.string = "\n❯ ";
    this->ps1 = ps1;
}
