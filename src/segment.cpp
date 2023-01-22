#include <unordered_map>
#include <regex>

#include "segment.hpp"
#include "segments/separator.hpp"
#include "segments/current_dir.hpp"
#include "segments/execution_time.hpp"
#include "segments/git_branch.hpp"

const std::unordered_map<std::string, std::string (*)(Context)> segment_map
{
    {"separator", &separator},
    {"current_dir", &current_dir},
    {"execution_time", &execution_time},
    {"git_branch", &git_branch}
};

std::string call_segment(std::string name, Context context)
{
    std::string result = "";
    auto pair = segment_map.find(name);
    if (pair != segment_map.end())
    {
        result = (*pair).second(context);
    }
    return result;
}

std::string execute_command(std::string command)
{
    auto strip = [](std::string &result)
    {
        std::string patterns[] = {
            "^\\s*",
            "\\s*$"
        };
        for (const std::string &pattern : patterns)
        {
            result = std::regex_replace(result, std::regex(pattern), "");
        }
    };
    std::string result = "";
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    pclose(pipe);
    strip(result);
    return result;
}
