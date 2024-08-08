#ifndef BLAZE_MODULES_DURATION_HPP_INCLUDED
#define BLAZE_MODULES_DURATION_HPP_INCLUDED

#include <string>
#include <cmath>

#include "src/context.hpp"

// Times in seconds
#define HOUR 3600
#define MINUTE 60
#define SECOND 1

// Execution time of the last command (wall time)
std::string duration(const Context &context)
{
    auto subtract_time = [](double &from, const int unit)
    {
        int time = floor(from / unit);
        from -= time * unit;
        return time;
    };
    std::string result = "";
    const Config &config = context.config;
    const float precision = std::pow(10, -config.duration.precision);
    const double start_time = context.args.start_time;
    const double finish_time = context.args.finish_time;
    double diff = round((finish_time - start_time) / precision) * precision;
    if (diff < config.duration.display_from || start_time == 0 || finish_time == 0)
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
    if (seconds || fractional > config.duration.display_from)
    {
        result += hours || minutes ? " " : "";
        if (seconds + minutes * MINUTE + hours * HOUR < config.duration.display_fractional_until)
        {
            result += std::to_string(seconds);
            result += std::to_string(fractional).substr(1, 1 + config.duration.precision);
        }
        else
        {
            result += std::to_string((int)round(seconds + fractional));
        }
        result += 's';
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("duration")
{
    Context context;
    context.config.duration.precision = 1;
    context.config.duration.display_from = 1;
    context.config.duration.display_fractional_until = 5;
    context.args.start_time = 0;
    context.args.finish_time = 0;
    SUBCASE("empty if start time is zero")
    {
        context.args.start_time = 0;
        context.args.finish_time = 10;
        const std::string result = duration(context);
        CHECK(result == "");
    }
    SUBCASE("empty if finish time is zero")
    {
        context.args.start_time = 10;
        context.args.finish_time = 0;
        const std::string result = duration(context);
        CHECK(result == "");
    }
    SUBCASE("empty if less than display_from")
    {
        context.config.duration.display_from = 10;
        context.args.start_time = 1;
        context.args.finish_time = 10;
        const std::string result = duration(context);
        CHECK(result == "");
    }
    SUBCASE("correct fractional")
    {
        context.config.duration.precision = 4;
        context.config.duration.display_from = 1;
        context.args.start_time = 3.5;
        context.args.finish_time = 5.512345;
        const std::string result = duration(context);
        CHECK(result == "2.0123s");
    }
    SUBCASE("skip fractional above display_fractional_until")
    {
        context.config.duration.precision = 4;
        context.config.duration.display_fractional_until = 5;
        context.args.start_time = 1;
        context.args.finish_time = 6.412345;
        const std::string result = duration(context);
        CHECK(result == "5s");
    }
    SUBCASE("correct seconds")
    {
        context.config.duration.precision = 1;
        context.args.start_time = 3;
        context.args.finish_time = 30.4;
        const std::string result = duration(context);
        CHECK(result == "27s");
    }
    SUBCASE("correct minutes")
    {
        context.args.start_time = 3;
        context.args.finish_time = 300.4;
        const std::string result = duration(context);
        CHECK(result == "4m 57s");
    }
    SUBCASE("correct hours")
    {
        context.args.start_time = 3;
        context.args.finish_time = 5900.4;
        const std::string result = duration(context);
        CHECK(result == "1h 38m 17s");
    }
}

#endif

#endif