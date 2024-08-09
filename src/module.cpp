#include <unordered_map>

#include "src/module.hpp"
#include "src/modules/date.hpp"
#include "src/modules/directory.hpp"
#include "src/modules/dummy.hpp"
#include "src/modules/duration.hpp"
#include "src/modules/git_branch.hpp"
#include "src/modules/git_commit.hpp"
#include "src/modules/git_status.hpp"
#include "src/modules/host.hpp"
#include "src/modules/shell.hpp"
#include "src/modules/status.hpp"
#include "src/modules/time.hpp"
#include "src/modules/user.hpp"

const std::unordered_map<std::string, std::string (*)(const Context &)> module_map
{
    {"date", &date},
    {"directory", &directory},
    {"fixed", &dummy},
    {"separator", &dummy},
    {"duration", &duration},
    {"git_branch", &git_branch},
    {"git_commit", &git_commit},
    {"git_status", &git_status},
    {"host", &host},
    {"shell", &shell},
    {"status", &status},
    {"time", &time},
    {"user", &user}
};

std::string call_module(const std::string &name, const Context &context)
{
    const auto pair = module_map.find(name);
    if (pair == module_map.end())
    {
        return "";
    }
    return (*pair).second(context);
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("call_module")
{
    Context context;
    SUBCASE("non-existent module")
    {
        CHECK(call_module("unknown", context) == "");
    }
    SUBCASE("existing module")
    {
        context.args.status = 255;
        CHECK(call_module("status", context) == "255");
    }
}

#endif
