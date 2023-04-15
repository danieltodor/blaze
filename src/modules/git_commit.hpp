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

#endif
