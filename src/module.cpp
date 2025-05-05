#include <unordered_map>

#include "module.hpp"
#include "modules/date.hpp"
#include "modules/directory.hpp"
#include "modules/duration.hpp"
#include "modules/git_branch.hpp"
#include "modules/git_commit.hpp"
#include "modules/git_status.hpp"
#include "modules/host.hpp"
#include "modules/shell.hpp"
#include "modules/status.hpp"
#include "modules/time.hpp"
#include "modules/user.hpp"

const std::unordered_map<std::string, std::string (*)(const Context &)> module_map
{
    {"date", &date},
    {"directory", &directory},
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
#ifdef TEST
#include "test.hpp"

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
