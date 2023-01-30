#include <unordered_map>

#include "segment.hpp"
#include "segments/separator.hpp"
#include "segments/directory.hpp"
#include "segments/execution_time.hpp"
#include "segments/git_branch.hpp"
#include "segments/git_status.hpp"

const std::unordered_map<std::string, std::string (*)(Context)> segment_map
{
    {"separator", &separator},
    {"directory", &directory},
    {"execution_time", &execution_time},
    {"git_branch", &git_branch},
    {"git_status", &git_status}
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
