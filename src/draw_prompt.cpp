#include <iostream>
#include <iterator>
#include <cmath>

#include "draw_prompt.hpp"
#include "colors.hpp"

std::string current_dir()
{
    std::string result = "";
    std::string PWD = std::getenv("PWD");
    std::string HOME = std::getenv("HOME");
    if (PWD.find(HOME) != std::string::npos)
    {
        PWD.replace(0, HOME.length(), "~");
    }
    result += bg(BLUE);
    result += fg(BLACK);
    result += ' ' + PWD + ' ';
    result += reset();
    result += fg(BLUE);
    result += "";
    result += reset();
    return result;
}

std::string execution_time(double start_time, double finish_time)
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
        stime += 's';
    }
    if (fractional && !hours && !minutes && !seconds)
    {
        stime += std::to_string(fractional).substr(1, EXECUTION_TIME_PRECISION + 1) + 's';
    }

    result += fg(YELLOW);
    result += " took ";
    result += stime;
    result += reset();
    return result;
}

std::string prompt()
{
    return "\n❯ ";
}

void draw_prompt(double start_time, double finish_time)
{
    std::string segments[] = {
        current_dir(),
        execution_time(start_time, finish_time),
        prompt()
    };
    std::string result;
    for (std::size_t i = 0; i < std::size(segments); i++)
    {
        result += segments[i];
    }
    std::cout << result;
}
