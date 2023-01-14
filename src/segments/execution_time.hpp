#ifndef BLAZE_SEGMENTS_EXECUTION_TIME_HPP_INCLUDED
#define BLAZE_SEGMENTS_EXECUTION_TIME_HPP_INCLUDED

#include <string>
#include <cmath>

#include "src/config.hpp"

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

std::string execution_time(Config conf, double start_time, double finish_time)
{
    auto subtract_time = [](double &from, int unit)
    {
        int time = floor(from / unit);
        from -= time * unit;
        return time;
    };

    std::string result = "";
    float precision = std::pow(10, -conf.global.execution_time_precision);
    double diff = round((finish_time - start_time) / precision) * precision;
    if (diff < conf.global.execution_time_display_from)
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
        stime += ' ' + std::to_string(hours) + 'h';
    }
    if (minutes)
    {
        stime += ' ' + std::to_string(minutes) + 'm';
    }
    if (seconds)
    {
        stime += ' ' + std::to_string(seconds);
        if (seconds + minutes * MINUTE + hours * HOUR < conf.global.execution_time_display_fractional_until)
        {
            stime += std::to_string(fractional).substr(1, 1 + conf.global.execution_time_precision);
        }
        stime += 's';
    }
    if (!hours && !minutes && !seconds)
    {
        stime += ' ' + std::to_string(fractional).substr(0, 2 + conf.global.execution_time_precision) + 's';
    }
    result += "took";
    result += stime;
    return result;
}

#endif
