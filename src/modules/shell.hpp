#ifndef BLAZE_MODULES_SHELL_HPP_INCLUDED
#define BLAZE_MODULES_SHELL_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Currently active shell
std::string shell(const Context &context)
{
    return context.args.shell;
}

#endif
