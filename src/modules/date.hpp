#ifndef BLAZE_MODULES_DATE_HPP_INCLUDED
#define BLAZE_MODULES_DATE_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string date(const Context &context)
{
    std::string result = "";
    result += format_time(context.time_structure, "%x");
    return result;
}

#endif
