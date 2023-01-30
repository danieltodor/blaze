#ifndef BLAZE_MODULES_GIT_STATUS_HPP_INCLUDED
#define BLAZE_MODULES_GIT_STATUS_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string git_status(Context context)
{
    std::string result = "";
    Config &config = context.config;
    if (!context.git_repository_detected)
    {
        return result;
    }
    std::string status = execute_command("git status --porcelain");
    if (!execute_command("git stash list").empty())
    {
        result += config.git_status.stashed;
    }
    if (regex_search(status, {"^.\? "}))
    {
        result += config.git_status.untracked;
    }
    if (regex_search(status, {"^.D "}))
    {
        result += config.git_status.deleted;
    }
    if (regex_search(status, {"^.R "}))
    {
        result += config.git_status.renamed;
    }
    if (regex_search(status, {"^.M "}))
    {
        result += config.git_status.modified;
    }
    if (regex_search(status, {"^M. "}))
    {
        result += config.git_status.staged;
    }
    return result;
}

#endif
