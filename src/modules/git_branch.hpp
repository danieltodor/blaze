#ifndef BLAZE_MODULES_GIT_BRANCH_HPP_INCLUDED
#define BLAZE_MODULES_GIT_BRANCH_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Active branch in the repository
std::string git_branch(const Context &context)
{
    std::string result = "";
    if (!context.git_repository_detected)
    {
        return result;
    }
    const Config &config = context.config;
    result += execute_command("git name-rev --name-only HEAD");
    strip(result);
    regex_replace(
        result,
        {
            "[~^]\\d*$",
            "^tags\\/"
        },
        ""
    );
    if (!regex_find_all(result, config.git_branch.ignore).empty())
    {
        result = "";
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("git_branch")
{
    Context context;
    SUBCASE("not a repository")
    {
        context.git_repository_detected = false;
        const std::string result = git_branch(context);
        CHECK(result == "");
    }
    SUBCASE("branch")
    {
        context.HOME = get_env("HOME");
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        const std::string result = git_branch(context);
        CHECK(result == "master");
    }
}

#endif

#endif
