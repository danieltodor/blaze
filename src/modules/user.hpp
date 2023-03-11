#ifndef BLAZE_MODULES_USER_HPP_INCLUDED
#define BLAZE_MODULES_USER_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string user(const Context &context)
{
    (void)context;
    std::string result = "";
    result += get_env("USER");
    return result;
}

#endif
