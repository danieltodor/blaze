#ifndef BLAZE_SEGMENTS_EXECUTION_TIME_HPP_INCLUDED
#define BLAZE_SEGMENTS_EXECUTION_TIME_HPP_INCLUDED

#include <string>
#include <cmath>

#include "src/context.hpp"

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

std::string execution_time(Context context)
{
    auto subtract_time = [](double &from, int unit)
    {
        int time = floor(from / unit);
        from -= time * unit;
        return time;
    };

    std::string result = "";
    Config &config = context.config;
    float precision = std::pow(10, -config.execution_time.precision);
    double diff = round((context.finish_time - context.start_time) / precision) * precision;
    if (diff < config.execution_time.display_from)
    {
        return result;
    }

    double &remaining = diff;
    int hours = subtract_time(remaining, HOUR);
    int minutes = subtract_time(remaining, MINUTE);
    int seconds = subtract_time(remaining, SECOND);
    double fractional = round(remaining / precision) * precision;

    if (hours)
    {
        result += std::to_string(hours) + 'h';
    }
    if (minutes)
    {
        result += hours ? " " : "";
        result += std::to_string(minutes) + 'm';
    }
    if (seconds)
    {
        result += hours || minutes ? " " : "";
        result += std::to_string(seconds);
        if (seconds + minutes * MINUTE + hours * HOUR < config.execution_time.display_fractional_until)
        {
            result += std::to_string(fractional).substr(1, 1 + config.execution_time.precision);
        }
        result += 's';
    }
    if (!hours && !minutes && !seconds)
    {
        result += std::to_string(fractional).substr(0, 2 + config.execution_time.precision) + 's';
    }
    return result;
}

#endif
