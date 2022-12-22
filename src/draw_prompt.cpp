#include <iostream>
#include <sys/ioctl.h>

#include "draw_prompt.hpp"
#include "colors.hpp"
#include "segments/segments.hpp"

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

artist::artist(std::string shell)
{
    config conf;
    this->conf = conf;
    this->shell = shell;
}

unsigned short get_col()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

bool level_changes(std::size_t i, config c)
{
    return i < c.segments.size() - 1 && c.segments[i + 1].level > c.current_sgm.level ? true : false;
}

bool end_reached(std::size_t i, config c)
{
    return i == c.segments.size() - 1 ? true : false;
}

void draw_prompt(std::string shell, double start_time, double finish_time)
{
    artist art(shell);
    std::string result;
    std::string temp;
    std::string left;
    std::string right;
    unsigned short length = 0;
    unsigned short extra_length = 0;
    for (std::size_t i = 0; i < art.conf.segments.size(); i++)
    {
        art.conf.current_sgm = art.conf.segments[i];
        art.conf.prev_sgm = art.conf.get_previous_segment(i);
        art.conf.next_sgm = art.conf.get_next_segment(i);
        temp += art.pre();
        temp += execute_segment(art.conf.current_sgm.name, start_time, finish_time);
        temp += art.post();
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
        if (level_changes(i, art.conf) || end_reached(i, art.conf))
        {
            result += left;
            left = "";
            if (right != "")
            {
                result += multiple(get_col() - length - extra_length, "─");
                result += right;
                right = "";
            }
            length = 0;
            extra_length = 0;
            if (level_changes(i, art.conf))
            {
                result += '\n';
            }
        }
    }
    result += art.prompt();
    std::cout << result;
}
