#include <unordered_map>

#include "src/module.hpp"
#include "src/modules/separator.hpp"
#include "src/modules/directory.hpp"
#include "src/modules/execution_time.hpp"
#include "src/modules/git_branch.hpp"
#include "src/modules/git_status.hpp"
#include "src/modules/date.hpp"
#include "src/modules/time.hpp"
#include "src/modules/user.hpp"
#include "src/modules/host.hpp"

const std::unordered_map<std::string, std::string (*)(const Context &)> module_map
{
    {"separator", &separator},
    {"directory", &directory},
    {"execution_time", &execution_time},
    {"git_branch", &git_branch},
    {"git_status", &git_status},
    {"date", &date},
    {"time", &time},
    {"user", &user},
    {"host", &host}
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
