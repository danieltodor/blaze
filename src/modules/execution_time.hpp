#ifndef BLAZE_MODULES_EXECUTION_TIME_HPP_INCLUDED
#define BLAZE_MODULES_EXECUTION_TIME_HPP_INCLUDED

#include <string>
#include <cmath>

#include "src/context.hpp"

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

// Execution time of the last command (wall time)
std::string execution_time(const Context &context)
{
    auto subtract_time = [](double &from, const int unit)
    {
        int time = floor(from / unit);
        from -= time * unit;
        return time;
    };

    std::string result = "";
    const Config &config = context.config;
    const float precision = std::pow(10, -config.execution_time.precision);
    const double start_time = std::stod(context.args.start_time);
    const double finish_time = std::stod(context.args.finish_time);
    double diff = round((finish_time - start_time) / precision) * precision;
    if (diff < config.execution_time.display_from)
    {
        return result;
    }

    const int hours = subtract_time(diff, HOUR);
    const int minutes = subtract_time(diff, MINUTE);
    const int seconds = subtract_time(diff, SECOND);
    const double fractional = round(diff / precision) * precision;

    if (hours)
    {
        result += std::to_string(hours) + 'h';
    }
    if (minutes)
    {
        result += hours ? " " : "";
        result += std::to_string(minutes) + 'm';
    }
    if (seconds || fractional > config.execution_time.display_from)
    {
        result += hours || minutes ? " " : "";
        if (seconds + minutes * MINUTE + hours * HOUR < config.execution_time.display_fractional_until)
        {
            result += std::to_string(seconds);
            result += std::to_string(fractional).substr(1, 1 + config.execution_time.precision);
        }
        else
        {
            result += std::to_string((int)round(seconds + fractional));
        }
        result += 's';
    }
    return result;
}

#endif
