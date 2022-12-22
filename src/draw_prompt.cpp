#include <iostream>
#include <cmath>
#include <sys/ioctl.h>

#include "draw_prompt.hpp"
#include "colors.hpp"

std::string multiple(int n, std::string c)
{
    std::string result = "";
    for (int i = 0; i < n; i++)
    {
        result += c;
    }
    return result;
}

std::string artist::pre()
{
    std::string result = "";
    result += reset();
    if (this->conf.prev_sgm.end_char == "")
    {
        result += bg(this->conf.prev_sgm.background);
    }
    result += fg(this->conf.current_sgm.background);
    result += this->conf.current_sgm.start_char;
    result += bg(this->conf.current_sgm.background);
    result += fg(this->conf.current_sgm.foreground);
    return result;
}

std::string artist::post()
{
    std::string result = "";
    result += reset();
    if (this->conf.next_sgm.start_char == "")
    {
        result += bg(this->conf.next_sgm.background);
    }
    result += fg(this->conf.current_sgm.background);
    result += this->conf.current_sgm.end_char;
    result += reset();
    return result;
}

std::string artist::current_dir()
{
    std::string result = "";
    std::string PWD = std::getenv("PWD");
    std::string HOME = std::getenv("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += this->pre();
    result += ' ' + PWD + ' ';
    result += this->post();
    return result;
}

std::string artist::execution_time(double start_time, double finish_time)
{
    auto subtract_time = [](double &from, int unit)
    {
        int time = floor(from / unit);
        from -= time * unit;
        return time;
    };

    std::string result = "";
    float precision = std::pow(10, -EXECUTION_TIME_PRECISION);
    double diff = round((finish_time - start_time) / precision) * precision;
    if (diff < EXECUTION_TIME_DISPLAY_FROM)
    {
        return result;
    }

    std::string stime = "";
    double &remaining = diff;
    int hours = subtract_time(remaining, HOUR);
    int minutes = subtract_time(remaining, MINUTE);
    int seconds = subtract_time(remaining, SECOND);
    double fractional = round(remaining / precision) * precision;

    if (hours)
    {
        stime += std::to_string(hours) + "h ";
    }
    if (minutes)
    {
        stime += std::to_string(minutes) + "m ";
    }
    if (seconds)
    {
        stime += std::to_string(seconds);
        if (fractional && !hours && !minutes && seconds < EXECUTION_TIME_DISPLAY_FRACTIONAL_UNTIL)
        {
            stime += std::to_string(fractional).substr(1, EXECUTION_TIME_PRECISION + 1);
        }
        stime += "s ";
    }
    if (fractional && !hours && !minutes && !seconds)
    {
        stime += std::to_string(fractional).substr(1, EXECUTION_TIME_PRECISION + 1) + "s ";
    }
    result += this->pre();
    result += " took ";
    result += stime;
    result += this->post();
    return result;
}

std::string artist::prompt()
{
    std::string result = "";
    if ('\n' == this->conf.prompt.fixed[0])
    {
        result += '\n';
        this->conf.prompt.fixed = this->conf.prompt.fixed.substr(1);
    }
    result += bg(this->conf.prompt.background);
    result += fg(this->conf.prompt.foreground);
    result += this->conf.prompt.fixed;
    result += reset();
    return result;
}

artist::artist(std::string shell, config conf)
{
    this->shell = shell;
    this->conf = conf;
}

unsigned short get_col()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

void draw_prompt(std::string shell, double start_time, double finish_time)
{
    config conf;
    artist art(shell, conf);
    std::string result;
    std::string temp;
    std::string left;
    std::string right;
    std::string func;
    unsigned short length = 0;
    unsigned short extra_length = 0;
    for (std::size_t i = 0; i < art.conf.segments.size(); i++)
    {
        art.conf.current_sgm = art.conf.segments[i];
        art.conf.prev_sgm = art.conf.get_previous_segment(i);
        art.conf.next_sgm = art.conf.get_next_segment(i);
        func = art.conf.current_sgm.name;
        if (func == "current_dir")
        {
            temp += art.current_dir();
        }
        else if (func == "execution_time")
        {
            temp += art.execution_time(start_time, finish_time);
        }
        if (art.conf.current_sgm.side == "right")
        {
            right += temp;
        }
        else
        {
            left += temp;
        }
        if (art.conf.current_sgm.start_char != ""){extra_length++;}
        if (art.conf.current_sgm.end_char != ""){extra_length++;}
        length += temp.length() - art.pre().length() - art.post().length();
        temp = "";
    }
    result += left;
    result += multiple(get_col() - length - extra_length, "─");
    result += right;
    result += art.prompt();
    std::cout << result;
}
