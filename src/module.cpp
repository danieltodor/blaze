#include <unordered_map>

#include "src/module.hpp"
#include "src/modules/dummy.hpp"
#include "src/modules/directory.hpp"
#include "src/modules/duration.hpp"
#include "src/modules/exit_status.hpp"
#include "src/modules/git_branch.hpp"
#include "src/modules/git_commit.hpp"
#include "src/modules/git_status.hpp"
#include "src/modules/date.hpp"
#include "src/modules/time.hpp"
#include "src/modules/user.hpp"
#include "src/modules/host.hpp"
#include "src/modules/shell.hpp"

const std::unordered_map<std::string, std::string (*)(const Context &)> module_map
{
    {"fixed", &dummy},
    {"separator", &dummy},
    {"directory", &directory},
    {"duration", &duration},
    {"exit_status", &exit_status},
    {"git_branch", &git_branch},
    {"git_commit", &git_commit},
    {"git_status", &git_status},
    {"date", &date},
    {"time", &time},
    {"user", &user},
    {"host", &host},
    {"shell", &shell}
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
