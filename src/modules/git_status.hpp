#pragma once

#include <string>

#include "src/types.hpp"
#include "src/context.hpp"
#include "src/util.hpp"

// Symbols indicating the current state of the repository
std::string git_status(const Context &context)
{
    if (!context.git_repository_detected)
    {
        return "";
    }
    const Config &config = context.config;
    std::string result = "";
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
    StringVector ahead_behind = regex_find_all(status, {"(?<=^# branch\\.ab \\+).*?(?=\n)"});
    if (!ahead_behind.empty())
    {
        ahead_behind = split(ahead_behind.at(0), " -");
        add_status(std::stoul(ahead_behind.at(0)), config.git_status.ahead);
        add_status(std::stoul(ahead_behind.at(1)), config.git_status.behind);
    }
    const StringVector stashed = regex_find_all(status, {"(?<=^# stash )\\d+"});
    if (!stashed.empty())
    {
        add_status(std::stoul(stashed.at(0)), config.git_status.stashed);
    }
    add_status(regex_find_all(status, {"^\\? "}).size(), config.git_status.untracked);
    add_status(regex_find_all(status, {"^u "}).size(), config.git_status.conflicted);
    add_status(regex_find_all(status, {"^\\d .M "}).size(), config.git_status.modified);
    add_status(regex_find_all(status, {"^\\d [AM]. "}).size(), config.git_status.staged);
    add_status(regex_find_all(status, {"^\\d R. "}).size(), config.git_status.renamed);
    add_status(regex_find_all(status, {"^\\d .D "}).size(), config.git_status.deleted);
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

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "src/test.hpp"

TEST_CASE("git_status")
{
    Context context;
    SUBCASE("not a repository")
    {
        context.git_repository_detected = false;
        CHECK(git_status(context) == "");
    }
    SUBCASE("status")
    {
        context.HOME = get_env("HOME");
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        CHECK(git_status(context).length() > 0);
    }
}

#endif
