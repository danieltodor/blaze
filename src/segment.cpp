#include "segment.hpp"
#include "segments/current_dir.hpp"
#include "segments/execution_time.hpp"

std::string call_segment(std::string name, Config config, double start_time, double finish_time)
{
    std::string result = "";
    if (name == "current_dir") {result = current_dir();}
    else if (name == "execution_time") {result = execution_time(config, start_time, finish_time);}
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
