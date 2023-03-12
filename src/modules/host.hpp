#ifndef BLAZE_MODULES_HOST_HPP_INCLUDED
#define BLAZE_MODULES_HOST_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Hostname
std::string host(const Context &context)
{
    (void)context;
    std::string result = "";
    result += execute_command("cat /etc/hostname");
    strip(result);
    return result;
}

#endif
