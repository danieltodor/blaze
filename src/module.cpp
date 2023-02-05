#include <unordered_map>

#include "module.hpp"
#include "modules/separator.hpp"
#include "modules/directory.hpp"
#include "modules/execution_time.hpp"
#include "modules/git_branch.hpp"
#include "modules/git_status.hpp"
#include "modules/date.hpp"
#include "modules/time.hpp"

const std::unordered_map<std::string, std::string (*)(const Context &)> module_map
{
    {"separator", &separator},
    {"directory", &directory},
    {"execution_time", &execution_time},
    {"git_branch", &git_branch},
    {"git_status", &git_status},
    {"date", &date},
    {"time", &time}
};

std::string call_module(const std::string &name, const Context &context)
{
    std::string result = "";
    const auto pair = module_map.find(name);
    if (pair != module_map.end())
    {
        result = (*pair).second(context);
    }
    return result;
}
