#include <unordered_map>

#include "src/module.hpp"
#include "src/modules/dummy.hpp"
#include "src/modules/directory.hpp"
#include "src/modules/execution_time.hpp"
#include "src/modules/git_branch.hpp"
#include "src/modules/git_status.hpp"
#include "src/modules/date.hpp"
#include "src/modules/time.hpp"
#include "src/modules/user.hpp"
#include "src/modules/host.hpp"
#include "src/modules/exit_status.hpp"

const std::unordered_map<std::string, std::string (*)(const Context &)> module_map
{
    {"fixed", &dummy},
    {"separator", &dummy},
    {"directory", &directory},
    {"execution_time", &execution_time},
    {"git_branch", &git_branch},
    {"git_status", &git_status},
    {"date", &date},
    {"time", &time},
    {"user", &user},
    {"host", &host},
    {"exit_status", &exit_status}
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

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("call_module")
{
    Context context;
    context.args.exit_status = "123";
    const std::string result = call_module("exit_status", context);
    CHECK(result.find("123") != std::string::npos);
}

#endif
