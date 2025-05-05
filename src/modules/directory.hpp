#pragma once

#include <string>

#include "context.hpp"
#include "util.hpp"

// Current working directory
std::string directory(const Context &context)
{
    std::string result = "";
    const Config &config = context.config;
    const std::string &HOME = context.HOME;
    const std::string &PWD = context.PWD;
    result += PWD;
    if (result.find(HOME) != std::string::npos)
    {
        result.replace(0, HOME.length(), "~");
    }
    if (config.directory.from_repository && context.git_repository_detected)
    {
        std::string git_repository_path = execute_command("git rev-parse --show-toplevel");
        if (git_repository_path.find(HOME) != std::string::npos)
        {
            git_repository_path.replace(0, HOME.length(), "~");
        }
        const std::size_t index = find_nth_occurrence(git_repository_path, "/", 1, true);
        result.replace(0, index + 1, "");
    }
    if (config.directory.limit > 0)
    {
        const std::size_t index = find_nth_occurrence(result, "/", config.directory.limit, true);
        if (index != std::string::npos)
        {
            result.replace(0, index, "...");
        }
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("directory")
{
    Context context;
    context.PWD = "/a/b/c";
    context.config.directory.limit = 4;
    context.config.directory.from_repository = false;
    SUBCASE("replace home with ~")
    {
        context.HOME = "/a";
        CHECK(directory(context) == "~/b/c");
    }
    SUBCASE("PWD when not in home directory")
    {
        context.HOME = "/d";
        CHECK(directory(context) == "/a/b/c");
    }
    SUBCASE("from repository")
    {
        context.PWD = get_env("PWD");
        context.config.directory.from_repository = true;
        context.git_repository_detected = true;
        CHECK(directory(context) == "blaze");
    }
    SUBCASE("from repository inside subdirectory")
    {
        context.HOME = get_env("HOME");
        context.PWD = get_env("PWD") + "/a/b/c";
        context.config.directory.from_repository = true;
        context.git_repository_detected = true;
        CHECK(directory(context) == "blaze/a/b/c");
    }
    SUBCASE("from repository directory limit")
    {
        context.HOME = get_env("HOME");
        context.PWD = get_env("PWD") + "/a/b/c/d/e/f";
        context.config.directory.from_repository = true;
        context.git_repository_detected = true;
        CHECK(directory(context) == ".../c/d/e/f");
    }
    SUBCASE("directory limit reached")
    {
        context.HOME = "/a";
        context.PWD = "/a/b/c/d/e";
        CHECK(directory(context) == ".../b/c/d/e");
    }
    SUBCASE("directory limit not reached")
    {
        context.HOME = "/a";
        context.PWD = "/a/b/c/d/e";
        context.config.directory.limit = 5;
        CHECK(directory(context) == "~/b/c/d/e");
    }
}

#endif
