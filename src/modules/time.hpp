#pragma once

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Current time
std::string time(const Context &context)
{
    const Config &config = context.config;
    return format_time(context.time_structure, config.time.format);
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "src/test.hpp"

TEST_CASE("time")
{
    Context context;
    context.time_structure.tm_gmtoff = 7200;
    context.time_structure.tm_isdst = 1;
    context.time_structure.tm_yday = 215;
    context.time_structure.tm_wday = 6;
    context.time_structure.tm_year = 124;
    context.time_structure.tm_mon = 7;
    context.time_structure.tm_mday = 3;
    context.time_structure.tm_hour = 20;
    context.time_structure.tm_min = 4;
    context.time_structure.tm_sec = 34;
    SUBCASE("correct time %R format")
    {
        context.config.time.format = "%R";
        CHECK(time(context) == "20:04");
    }
    SUBCASE("correct time %T format")
    {
        context.config.time.format = "%T";
        CHECK(time(context) == "20:04:34");
    }
}

#endif
