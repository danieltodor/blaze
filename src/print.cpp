#include <iostream>
#include <sys/ioctl.h>

#include "print.hpp"
#include "config.hpp"
#include "color.hpp"
#include "segment.hpp"

std::string pre(config conf, segment current_segment, segment previous_segment)
{
    std::string result = "";
    result += reset();
    if (previous_segment.suffix.empty())
    {
        result += background(previous_segment.background);
    }
    result += foreground(current_segment.background);
    result += current_segment.prefix;
    result += background(current_segment.background);
    result += foreground(current_segment.foreground);
    result += conf.glob.padding;
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
    result += foreground(conf.conn.foreground);
    result += multiple(length, conf.conn.character);
    result += reset();
    return result;
}

std::string post(config conf, segment current_segment, segment next_segment)
{
    std::string result = "";
    result += reset();
    result += background(current_segment.background);
    result += foreground(current_segment.foreground);
    result += conf.glob.padding;
    result += reset();
    if (next_segment.prefix.empty())
    {
        result += background(next_segment.background);
    }
    result += foreground(current_segment.background);
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
    result += foreground(conf.ps1.foreground);
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

void print_all(double start_time, double finish_time)
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
        if (!current_sgm.name.empty())
        {
            temp += call_segment(current_sgm.name, conf, start_time, finish_time);
        }
        else if (!current_sgm.execute.empty())
        {
            temp += execute_segment(current_sgm.execute);
        }
        temp += post(conf, current_sgm, next_sgm);
        if (current_sgm.align == "right")
        {
            right += temp;
        }
        else
        {
            left += temp;
        }
        length += temp.length() - pre(conf, current_sgm, prev_sgm).length() - post(conf, current_sgm, next_sgm).length();
        if (!current_sgm.prefix.empty()) {length += current_sgm.prefix.length() - 2;}
        if (!current_sgm.suffix.empty()) {length += current_sgm.suffix.length() - 2;}
        if (!conf.glob.padding.empty()) {length += conf.glob.padding.length() * 2;}
        if (level_changes(i, conf) || end_reached(i, conf))
        {
            result += left;
            left = "";
            if (!right.empty())
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
