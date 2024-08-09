#ifndef BLAZE_MODULES_GIT_COMMIT_HPP_INCLUDED
#define BLAZE_MODULES_GIT_COMMIT_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Commit hash
std::string git_commit(const Context &context)
{
    std::string result = "";
    if (!context.git_repository_detected)
    {
        return result;
    }
    const Config &config = context.config;
    result += execute_command("git rev-parse HEAD");
    strip(result);
    if (config.git_commit.length > 0)
    {
        result = result.substr(0, config.git_commit.length);
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("git_commit")
{
    Context context;
    SUBCASE("not a repository")
    {
        context.git_repository_detected = false;
        CHECK(git_commit(context) == "");
    }
    SUBCASE("commit trimmed")
    {
        context.HOME = get_env("HOME");
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.config.git_commit.length = 8;
        CHECK(git_commit(context).length() == 8);
    }
    SUBCASE("commit not trimmed")
    {
        context.HOME = get_env("HOME");
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.config.git_commit.length = 0;
        CHECK(git_commit(context).length() > 8);
    }
}

#endif

#endif
