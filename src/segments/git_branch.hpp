#ifndef BLAZE_SEGMENTS_GIT_BRANCH_HPP_INCLUDED
#define BLAZE_SEGMENTS_GIT_BRANCH_HPP_INCLUDED

#include <string>
#include <filesystem>

#include "src/context.hpp"
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
    regex_replace(result, {"\\(HEAD.*\\)"}, "");
    strip(result);
    if (regex_search(result, config.git_branch.ignore))
    {
        result = "";
    }
    return result;
}

#endif
