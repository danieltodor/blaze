#ifndef BLAZE_MODULES_GIT_STATUS_HPP_INCLUDED
#define BLAZE_MODULES_GIT_STATUS_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

std::string git_status(const Context &context)
{
    std::string result = "";
    if (!context.git_repository_detected)
    {
        return result;
    }
    const Config &config = context.config;
    auto add_status = [&config, &result](int count, const std::string &symbol)
    {
        if (count)
        {
            result += config.git_status.count ? std::to_string(count) : "";
            result += symbol;
        }
    };
    std::string status = execute_command("git status --porcelain");
    add_status(regex_count(status, {"^.\\? "}), config.git_status.untracked);
    add_status(regex_count(execute_command("git stash list"), {"stash@\\{"}), config.git_status.stashed);
    add_status(regex_count(status, {"^.M "}), config.git_status.modified);
    add_status(regex_count(status, {"^M. "}), config.git_status.staged);
    add_status(regex_count(status, {"^.R "}), config.git_status.renamed);
    add_status(regex_count(status, {"^.D "}), config.git_status.deleted);
    if (result.empty())
    {
        result += config.git_status.clean;
    }
    return result;
}

#endif
