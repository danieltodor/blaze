#ifndef BLAZE_EXECUTION_TIME_HPP_INCLUDED
#define BLAZE_EXECUTION_TIME_HPP_INCLUDED

#include <string>
#include <cmath>

// Settings
#define EXECUTION_TIME_PRECISION 1
#define EXECUTION_TIME_DISPLAY_FROM 0
#define EXECUTION_TIME_DISPLAY_FRACTIONAL_UNTIL 10

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

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
        stime += ' ' + std::to_string(hours) + 'h';
    }
    if (minutes)
    {
        stime += ' ' + std::to_string(minutes) + 'm';
    }
    if (seconds)
    {
        stime += ' ' + std::to_string(seconds);
        if (fractional && !hours && !minutes && seconds < EXECUTION_TIME_DISPLAY_FRACTIONAL_UNTIL)
        {
            stime += std::to_string(fractional).substr(1, 1 + EXECUTION_TIME_PRECISION);
        }
        stime += 's';
    }
    if (fractional && !hours && !minutes && !seconds)
    {
        stime += ' ' + std::to_string(fractional).substr(0, 2 + EXECUTION_TIME_PRECISION) + 's';
    }
    result += "took";
    result += stime;
    return result;
}

#endif
