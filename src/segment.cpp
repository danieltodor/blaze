#include <unordered_map>

#include "segment.hpp"
#include "segments/separator.hpp"
#include "segments/current_dir.hpp"
#include "segments/execution_time.hpp"

const std::unordered_map<std::string, std::string (*)(Context)> segment_map
{
    {"separator", &separator},
    {"current_dir", &current_dir},
    {"execution_time", &execution_time}
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

std::string execute_segment(std::string command)
{
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
    // Remove trailing newline
    if (result[result.length() - 1] == '\n')
    {
        result = result.substr(0, result.length() - 1);
    }
    return result;
}
