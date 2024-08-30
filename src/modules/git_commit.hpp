#ifndef BLAZE_MODULES_GIT_COMMIT_HPP_INCLUDED
#define BLAZE_MODULES_GIT_COMMIT_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Commit hash
std::string git_commit(const Context &context)
{
    const Config &config = context.config;
    if (!context.git_repository_detected)
    {
        return "";
    }
    else if (!context.git_repository_detached && !config.git_commit.show_when_attached)
    {
        return "";
    }
    std::string result = "";
    const int length = config.git_commit.length > 0 ? config.git_commit.length : 99;
    result += execute_command("git rev-parse --short=" + std::to_string(length) + " HEAD");
    strip(result);
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
        context.git_repository_detached = true;
        CHECK(git_commit(context) == "");
    }
    SUBCASE("hide if not detached")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = false;
        CHECK(git_commit(context) == "");
    }
    SUBCASE("show if not detached")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = false;
        context.config.git_commit.show_when_attached = true;
        CHECK(git_commit(context) != "");
    }
    SUBCASE("commit trimmed")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = true;
        context.config.git_commit.length = 8;
        CHECK(git_commit(context).length() == 8);
    }
    SUBCASE("commit not trimmed")
    {
        context.PWD = get_env("PWD");
        context.git_repository_detected = true;
        context.git_repository_detached = true;
        context.config.git_commit.length = 0;
        CHECK(git_commit(context).length() > 8);
    }
}

#endif

#endif
