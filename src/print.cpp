#include <iostream>
#include <sys/ioctl.h>
#include <vector>
#include <locale>
#include <codecvt>

#include "print.hpp"
#include "color.hpp"
#include "segment.hpp"
#include "util.hpp"

std::string get_padding(Config config, Segment current_segment)
{
    if (current_segment.name == "separator" && current_segment.padding == control_char)
    {
        return "";
    }
    return current_segment.padding != control_char ? current_segment.padding : config.global.padding;
}

std::string pre(Config config, Segment current_segment, Segment previous_segment)
{
    std::string result = "";
    result += reset();
    if (previous_segment.outer_suffix.empty())
    {
        result += background(previous_segment.background);
    }
    result += foreground(current_segment.background);
    result += current_segment.outer_prefix;
    result += background(current_segment.background);
    result += foreground(current_segment.foreground);
    result += get_padding(config, current_segment);
    result += current_segment.inner_prefix;
    if (current_segment.bold)
    {
        result += text_mode(BOLD);
    }
    if (current_segment.dim)
    {
        result += text_mode(DIM);
    }
    if (current_segment.italic)
    {
        result += text_mode(ITALIC);
    }
    if (current_segment.underline)
    {
        result += text_mode(UNDERLINE);
    }
    return result;
}

std::string middle(Config config, int length)
{
    auto multiple = [](int n, std::string c)
    {
        std::string result = "";
        for (int i = 0; i < n; i++)
        {
            result += c;
        }
        return result;
    };
    std::string result = "";
    result += reset();
    result += foreground(config.connector.foreground);
    result += multiple(length, config.connector.character);
    result += reset();
    return result;
}

std::string post(Config config, Segment current_segment, Segment next_segment)
{
    std::string result = "";
    result += reset();
    result += background(current_segment.background);
    result += foreground(current_segment.foreground);
    result += current_segment.inner_suffix;
    result += get_padding(config, current_segment);
    result += reset();
    if (next_segment.outer_prefix.empty())
    {
        result += background(next_segment.background);
    }
    result += foreground(current_segment.background);
    result += current_segment.outer_suffix;
    result += reset();
    return result;
}

std::string prompt(Config config)
{
    std::string result = "";
    result += reset();
    result += foreground(config.prompt.foreground);
    result += config.prompt.string;
    result += reset();
    return result;
}

unsigned short get_col()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

std::size_t get_length(std::vector<std::string> strings)
{
    std::size_t length = 0;
    for (const std::string &string : strings)
    {
        length += std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(string).size();
    }
    return length;
}

bool level_changes(std::size_t i, Config c)
{
    return i < c.segments.size() - 1 && c.segments[i + 1].level > c.segments[i].level ? true : false;
}

bool end_reached(std::size_t i, Config c)
{
    return i == c.segments.size() - 1 ? true : false;
}

void print_all(Context context)
{
    Config &config = context.config;
    std::string result;
    std::string temp;
    std::string left;
    std::string right;
    std::size_t length = 0;
    for (std::size_t i = 0; i < config.segments.size(); i++)
    {
        Segment current_segment = config.segments[i];
        Segment previous_segment = config.get_previous_segment(i);
        Segment next_segment = config.get_next_segment(i);
        if (!current_segment.name.empty())
        {
            temp += call_segment(current_segment.name, context);
        }
        else if (!current_segment.execute.empty())
        {
            temp += execute_command(current_segment.execute);
        }
        if (!temp.empty() || current_segment.name == "separator")
        {
            length += get_length({
                temp,
                get_padding(config, current_segment),
                get_padding(config, current_segment),
                current_segment.inner_prefix,
                current_segment.inner_suffix,
                current_segment.outer_prefix,
                current_segment.outer_suffix
            });
            temp.insert(0, pre(config, current_segment, previous_segment));
            temp.insert(temp.length(), post(config, current_segment, next_segment));
        }
        if (current_segment.align == "right")
        {
            right += temp;
        }
        else
        {
            left += temp;
        }
        if (level_changes(i, config) || end_reached(i, config))
        {
            result += left;
            left = "";
            if (!right.empty())
            {
                result += middle(config, get_col() - length);
                result += right;
                right = "";
            }
            length = 0;
        }
        if (level_changes(i, config))
        {
            result += '\n';
        }
        temp = "";
    }
    result += prompt(config);
    std::cout << result;
}
