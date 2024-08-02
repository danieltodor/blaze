#ifndef BLAZE_MODULES_EXIT_STATUS_HPP_INCLUDED
#define BLAZE_MODULES_EXIT_STATUS_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Exit status of the last command
std::string exit_status(const Context &context)
{
    std::string result = "";
    if (context.args.exit_status != 0 || !context.config.exit_status.non_zero_only)
    {
        result += std::to_string(context.args.exit_status);
    }
    return result;
}

#endif
