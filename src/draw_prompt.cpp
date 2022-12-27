#include <iostream>
#include <sys/ioctl.h>

#include "draw_prompt.hpp"
#include "config.hpp"
#include "colors.hpp"
#include "segments/segments.hpp"

std::string pre(config conf, segment current_segment, segment previous_segment)
{
    std::string result = "";
    result += reset();
    if (previous_segment.suffix == "")
    {
        result += bg(previous_segment.background);
    }
    result += fg(current_segment.background);
    result += current_segment.prefix;
    result += bg(current_segment.background);
    result += fg(current_segment.foreground);
    result += conf.padding;
    return result;
}

std::string middle(config conf, int length)
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
    result += fg(conf.conn.foreground);
    result += multiple(length, conf.conn.character);
    result += reset();
    return result;
}

std::string post(config conf, segment current_segment, segment next_segment)
{
    std::string result = "";
    result += conf.padding;
    result += reset();
    if (next_segment.prefix == "")
    {
        result += bg(next_segment.background);
    }
    result += fg(current_segment.background);
    result += current_segment.suffix;
    result += reset();
    return result;
}

std::string ps1(config conf)
{
    std::string result = "";
    if ('\n' == conf.ps1.string[0])
    {
        result += '\n';
        conf.ps1.string = conf.ps1.string.substr(1);
    }
    result += fg(conf.ps1.foreground);
    result += conf.ps1.string;
    result += reset();
    return result;
}

unsigned short get_col()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

bool level_changes(std::size_t i, config c)
{
    return i < c.segments.size() - 1 && c.segments[i + 1].level > c.segments[i].level ? true : false;
}

bool end_reached(std::size_t i, config c)
{
    return i == c.segments.size() - 1 ? true : false;
}

void draw_prompt(double start_time, double finish_time)
{
    config conf;
    std::string result;
    std::string temp;
    std::string left;
    std::string right;
    unsigned short length = 0;
    for (std::size_t i = 0; i < conf.segments.size(); i++)
    {
        segment current_sgm = conf.segments[i];
        segment prev_sgm = conf.get_previous_segment(i);
        segment next_sgm = conf.get_next_segment(i);
        temp += pre(conf, current_sgm, prev_sgm);
        if (current_sgm.name != "")
        {
            temp += call_segment(current_sgm.name, start_time, finish_time);
        }
        else if (current_sgm.execute != "")
        {
            temp += execute_segment(current_sgm.execute);
        }
        temp += post(conf, current_sgm, next_sgm);
        if (current_sgm.side == "right")
        {
            right += temp;
        }
        else
        {
            left += temp;
        }
        length += temp.length() - pre(conf, current_sgm, prev_sgm).length() - post(conf, current_sgm, next_sgm).length();
        if (current_sgm.prefix != "") {length += current_sgm.prefix.length() - 2;}
        if (current_sgm.suffix != "") {length += current_sgm.suffix.length() - 2;}
        if (conf.padding != "") {length += conf.padding.length() * 2;}
        if (level_changes(i, conf) || end_reached(i, conf))
        {
            result += left;
            left = "";
            if (right != "")
            {
                result += middle(conf, get_col() - length);
                result += right;
                right = "";
            }
            length = 0;
        }
        if (level_changes(i, conf))
        {
            result += '\n';
        }
        temp = "";
    }
    result += ps1(conf);
    std::cout << result;
}
