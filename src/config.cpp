#include <iostream>

#include "config.hpp"
#include "toml.hpp"
#include "colors.hpp"

// Relative to ~
#define CONFIG_PATH "/.config/blaze/config.toml"

void config::parse_config()
{
    toml::table tbl;
    std::unordered_map<std::string, int> colors = color_map();
    std::string HOME = std::getenv("HOME");
    try
    {
        tbl = toml::parse_file(HOME + CONFIG_PATH);
        for (int i = 0; i < SEGMENT_COUNT; i++)
        {
            segment current;
            current.name = tbl["segment"][i]["name"].value_or("");
            current.side = tbl["segment"][i]["side"].value_or("left");
            current.start_char = tbl["segment"][i]["start_char"].value_or("");
            current.end_char = tbl["segment"][i]["end_char"].value_or("");
            current.background = colors[tbl["segment"][i]["background"].value_or("none")];
            current.foreground = colors[tbl["segment"][i]["foreground"].value_or("none")];
            this->segments[i] = current;
        }
        this->prompt.fixed = tbl["prompt"]["fixed"].value_or(" ");
        this->prompt.background = colors[tbl["prompt"]["background"].value_or("none")];
        this->prompt.foreground = colors[tbl["prompt"]["foreground"].value_or("none")];
    }
    catch (const toml::parse_error &err)
    {
        std::cout << "Failed to parse config: " << err;
    }
}

config::config()
{
    this->parse_config();
}
