#ifndef BLAZE_MODULES_DIRECTORY_HPP_INCLUDED
#define BLAZE_MODULES_DIRECTORY_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

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
        result.replace(0, index, "...");
    }
    if (config.directory.length > 0)
    {
        const std::size_t index = find_nth_occurrence(result, "/", config.directory.length, true);
        if (index != std::string::npos)
        {
            result.replace(0, index, "...");
        }
    }
    return result;
}

#endif
