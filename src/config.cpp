#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "config.hpp"
#include "toml.hpp"

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
    int i = 0;
    while (true)
    {
        if (!tbl["segment"][i])
        {
            break;
        }
        segment current;
        current.name = tbl["segment"][i]["name"].value_or("");
        current.execute = tbl["segment"][i]["execute"].value_or("");
        current.level = tbl["segment"][i]["level"].value_or(1);
        current.position = tbl["segment"][i]["position"].value_or(1);
        current.align = tbl["segment"][i]["align"].value_or("left");
        current.prefix = tbl["segment"][i]["prefix"].value_or("");
        current.suffix = tbl["segment"][i]["suffix"].value_or("");
        current.background = tbl["segment"][i]["background"].value_or("");
        current.foreground = tbl["segment"][i]["foreground"].value_or("");
        current.bold = tbl["segment"][i]["bold"].value_or(false);
        current.dim = tbl["segment"][i]["dim"].value_or(false);
        current.italic = tbl["segment"][i]["italic"].value_or(false);
        current.underline = tbl["segment"][i]["underline"].value_or(false);
        this->segments.push_back(current);
        i++;
    }
    this->ps1.string = tbl["prompt"]["string"].value_or(" ");
    this->ps1.foreground = tbl["prompt"]["foreground"].value_or("");
    this->conn.character = tbl["connector"]["character"].value_or(" ");
    this->conn.foreground = tbl["connector"]["foreground"].value_or("");
    this->padding = tbl["padding"].value_or(" ");
    this->execution_time_precision = tbl["execution_time_precision"].value_or(1);
    this->execution_time_display_from = tbl["execution_time_display_from"].value_or(0);
    this->execution_time_display_fractional_until = tbl["execution_time_display_fractional_until"].value_or(10);
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
    segment current_dir;
    current_dir.name = "current_dir";
    current_dir.level = 1;
    current_dir.position = 1;
    current_dir.align = "left";
    current_dir.background = "blue";
    current_dir.foreground = "black";
    current_dir.suffix = "";
    this->segments.push_back(current_dir);

    segment execution_time;
    execution_time.name = "execution_time";
    execution_time.level = 1;
    execution_time.position = 2;
    execution_time.align = "left";
    execution_time.foreground = "yellow";
    this->segments.push_back(execution_time);

    prompt ps1;
    ps1.string = "\n❯ ";
    this->ps1 = ps1;

    this->padding = " ";
    this->execution_time_precision = 1;
    this->execution_time_display_from = 2;
    this->execution_time_display_fractional_until = 10;
}
