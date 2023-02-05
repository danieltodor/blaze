#ifndef BLAZE_MODULES_TIME_HPP_INCLUDED
#define BLAZE_MODULES_TIME_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string time(const Context &context)
{
    std::string result = "";
    result += format_time(context.time_structure, "%X");
    return result;
}

#endif
