#pragma once

#include <string>
#include <cmath>

#include "context.hpp"

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
    const Config &config = context.config;
    const double precision = std::pow(10, -config.duration.precision);
    const double start_time = context.args.start_time;
    const double finish_time = context.args.finish_time;
    const double difference = round((finish_time - start_time) / precision) * precision;
    if (difference < config.duration.threshold || start_time <= 0 || finish_time <= 0)
    {
        return "";
    }
    std::string result = "";
    double remainder = difference;
    const int hours = subtract_time(remainder, HOUR);
    const int minutes = subtract_time(remainder, MINUTE);
    const int seconds = subtract_time(remainder, SECOND);
    const double fractional = round(remainder / precision) * precision;
    if (hours)
    {
        result += std::to_string(hours) + 'h';
    }
    if (minutes)
    {
        result += hours ? " " : "";
        result += std::to_string(minutes) + 'm';
    }
    if (seconds || fractional > 0)
    {
        result += hours || minutes ? " " : "";
        if (difference < config.duration.fractional_until)
        {
            result += std::to_string(seconds);
            result += std::to_string(fractional).substr(1, 1 + config.duration.precision);
        }
        else
        {
            result += std::to_string(static_cast<int>(round(seconds + fractional)));
        }
        result += 's';
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("duration")
{
    Context context;
    context.config.duration.precision = 1;
    context.config.duration.threshold = 1;
    context.config.duration.fractional_until = 5;
    context.args.start_time = 0;
    context.args.finish_time = 0;
    SUBCASE("empty if start time is zero")
    {
        context.args.start_time = 0;
        context.args.finish_time = 10;
        CHECK(duration(context) == "");
    }
    SUBCASE("empty if finish time is zero")
    {
        context.args.start_time = 10;
        context.args.finish_time = 0;
        CHECK(duration(context) == "");
    }
    SUBCASE("empty if less than threshold")
    {
        context.config.duration.threshold = 10;
        context.args.start_time = 1;
        context.args.finish_time = 10;
        CHECK(duration(context) == "");
    }
    SUBCASE("correct fractional")
    {
        context.config.duration.precision = 4;
        context.config.duration.threshold = 1;
        context.args.start_time = 3.5;
        context.args.finish_time = 5.512345;
        CHECK(duration(context) == "2.0123s");
    }
    SUBCASE("skip fractional above fractional_until")
    {
        context.config.duration.precision = 4;
        context.config.duration.fractional_until = 5;
        context.args.start_time = 1;
        context.args.finish_time = 6.412345;
        CHECK(duration(context) == "5s");
    }
    SUBCASE("correct rounding down")
    {
        context.config.duration.threshold = 1;
        context.config.duration.fractional_until = 5;
        context.args.start_time = 1;
        context.args.finish_time = 6.4;
        CHECK(duration(context) == "5s");
    }
    SUBCASE("correct rounding up")
    {
        context.config.duration.threshold = 1;
        context.config.duration.fractional_until = 5;
        context.args.start_time = 1;
        context.args.finish_time = 6.5;
        CHECK(duration(context) == "6s");
    }
    SUBCASE("correct seconds")
    {
        context.config.duration.precision = 1;
        context.args.start_time = 3;
        context.args.finish_time = 30.4;
        CHECK(duration(context) == "27s");
    }
    SUBCASE("correct minutes")
    {
        context.args.start_time = 3;
        context.args.finish_time = 300.4;
        CHECK(duration(context) == "4m 57s");
    }
    SUBCASE("correct hours")
    {
        context.args.start_time = 3;
        context.args.finish_time = 5900.4;
        CHECK(duration(context) == "1h 38m 17s");
    }
}

#endif
