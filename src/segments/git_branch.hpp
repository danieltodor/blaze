#ifndef BLAZE_SEGMENTS_GIT_BRANCH_HPP_INCLUDED
#define BLAZE_SEGMENTS_GIT_BRANCH_HPP_INCLUDED

#include <string>
#include <regex>
#include <filesystem>

#include "src/context.hpp"
#include "src/segment.hpp"

std::string git_branch(Context context)
{
    std::string result = "";
    std::string &PWD = context.PWD;
    if (!std::filesystem::is_directory(PWD + '/' + ".git"))
    {
        return result;
    }
    result += execute_command("git branch");
    std::string patterns[] = {
        "\\*",
        "\\s"
    };
    for (const std::string &pattern : patterns)
    {
        result = std::regex_replace(result, std::regex(pattern), "");
    }
    return result;
}

#endif
