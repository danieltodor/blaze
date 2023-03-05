#ifndef BLAZE_MODULES_GIT_BRANCH_HPP_INCLUDED
#define BLAZE_MODULES_GIT_BRANCH_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string git_branch(const Context &context)
{
    std::string result = "";
    if (!context.git_repository_detected)
    {
        return result;
    }
    const Config &config = context.config;
    result += execute_command("git name-rev --name-only HEAD 2>/dev/null");
    strip(result);
    regex_replace(
        result,
        {
            "[~^]\\d*$",
            "^tags\\/"
        },
        ""
    );
    if (regex_find_all(result, config.git_branch.ignore).empty())
    {
        result = "";
    }
    return result;
}

#endif
