#ifndef BLAZE_MODULES_SEPARATOR_HPP_INCLUDED
#define BLAZE_MODULES_SEPARATOR_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Can be used for additional separation between modules
std::string separator(const Context &context)
{
    (void)context;
    std::string result = "";
    return result;
}

#endif
