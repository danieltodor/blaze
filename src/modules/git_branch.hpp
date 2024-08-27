#ifndef BLAZE_MODULES_GIT_BRANCH_HPP_INCLUDED
#define BLAZE_MODULES_GIT_BRANCH_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Active branch in the repository
std::string git_branch(const Context &context)
{
    if (!context.git_repository_detected || context.git_repository_detached)
    {
        return "";
    }
    const Config &config = context.config;
    std::string result = "";
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
        context.git_repository_detached = false;
        CHECK(git_branch(context) == "");
    }
    SUBCASE("branch")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = false;
        CHECK(git_branch(context) == "master");
    }
    SUBCASE("hide detached")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = true;
        CHECK(git_branch(context) == "");
    }
    SUBCASE("ignored branch")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = false;
        context.config.git_branch.ignore = {"master"};
        CHECK(git_branch(context) == "");
    }
}

#endif

#endif
