#ifndef BLAZE_SEGMENTS_GIT_BRANCH_HPP_INCLUDED
#define BLAZE_SEGMENTS_GIT_BRANCH_HPP_INCLUDED

#include <string>
#include <filesystem>

#include "src/context.hpp"
#include "src/segment.hpp"
#include "src/utils.hpp"

std::string git_branch(Context context)
{
    std::string result = "";
    Config &config = context.config;
    std::string &PWD = context.PWD;
    if (!std::filesystem::is_directory(PWD + '/' + ".git"))
    {
        return result;
    }
    result += execute_command("git branch --contains HEAD --format '%(refname:lstrip=2)'");
    const std::string patterns[] = {
        "\\(HEAD.*\\)"
    };
    regex_replace(result, patterns, "");
    strip(result);
    for (const std::string &ignore : config.git_branch.ignore)
    {
        if (result == ignore)
        {
            result = "";
            break;
        }
    }
    return result;
}

#endif
