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
    auto add_status = [&config, &result](const std::size_t count, const std::string &symbol)
    {
        if (count && !symbol.empty())
        {
            result += config.git_status.count ? std::to_string(count) : "";
            result += symbol;
            result += config.git_status.spacing;
        }
    };
    std::string status = execute_command("git status --porcelain=v2 --branch --show-stash");
    std::vector<std::string> ahead_behind = regex_find_all(status, {"(?<=^# branch\\.ab \\+).*?(?=\n)"});
    if (!ahead_behind.empty())
    {
        ahead_behind = split(ahead_behind.at(0), " -");
        add_status(std::stoul(ahead_behind.at(0)), config.git_status.ahead);
        add_status(std::stoul(ahead_behind.at(1)), config.git_status.behind);
    }
    const std::vector<std::string> stashed = regex_find_all(status, {"(?<=^# stash )\\d+"});
    if (!stashed.empty())
    {
        add_status(std::stoul(stashed.at(0)), config.git_status.stashed);
    }
    add_status(regex_find_all(status, {"^\\? "}).size(), config.git_status.untracked);
    add_status(regex_find_all(status, {"^\\d+ .M "}).size(), config.git_status.modified);
    add_status(regex_find_all(status, {"^\\d+ M. "}).size(), config.git_status.staged);
    add_status(regex_find_all(status, {"^\\d+ R. "}).size(), config.git_status.renamed);
    add_status(regex_find_all(status, {"^\\d+ .D "}).size(), config.git_status.deleted);
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
