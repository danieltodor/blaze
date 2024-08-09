#ifndef BLAZE_MODULES_DATE_HPP_INCLUDED
#define BLAZE_MODULES_DATE_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Current date
std::string date(const Context &context)
{
    const Config &config = context.config;
    std::string result = "";
    result += format_time(context.time_structure, config.date.format);
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("date")
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
    SUBCASE("correct date %F format")
    {
        context.config.date.format = "%F";
        CHECK(date(context) == "2024-08-03");
    }
    SUBCASE("correct date %T format")
    {
        context.config.date.format = "%T";
        CHECK(date(context) == "20:04:34");
    }
}

#endif

#endif
