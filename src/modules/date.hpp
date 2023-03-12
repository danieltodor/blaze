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

#endif
