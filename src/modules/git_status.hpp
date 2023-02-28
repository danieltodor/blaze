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
            result += config.git_status.spacing;
        }
    };
    std::string status = execute_command("git status --porcelain=v2 --branch --show-stash");
    add_status(regex_count(status, {"^# branch\\.ab \\+[1-9]{1,} "}), config.git_status.ahead);
    add_status(regex_count(status, {"^# branch\\.ab \\+\\d+ -[1-9]{1,}"}), config.git_status.behind);
    add_status(regex_count(status, {"^\\? "}), config.git_status.untracked);
    add_status(regex_count(status, {"^# stash"}), config.git_status.stashed);
    add_status(regex_count(status, {"^\\d+ .M "}), config.git_status.modified);
    add_status(regex_count(status, {"^\\d+ M. "}), config.git_status.staged);
    add_status(regex_count(status, {"^\\d+ R. "}), config.git_status.renamed);
    add_status(regex_count(status, {"^\\d+ .D "}), config.git_status.deleted);
    if (result.empty())
    {
        result += config.git_status.clean;
    }
    else
    {
        result = result.substr(0, result.length() - config.git_status.spacing.length());
    }
    return result;
}

#endif
