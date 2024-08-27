#include "src/context.hpp"
#include "src/util.hpp"

Context get_context(int argc, char *argv[])
{
    bool git_repository_detected = false;
    bool git_repository_detached = false;
    check_git_repository(git_repository_detected, git_repository_detached);
    Context context = {
        argparse::parse<Args>(argc, argv),
        get_env("HOME"),
        get_env("PWD"),
        git_repository_detected,
        git_repository_detached,
        get_current_time(),
        get_config()
    };
    return context;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("get_context")
{
    const int argc = 2;
    const char *argv[] = {"name", "bash"};
    Context context = get_context(argc, (char **)argv);
    CHECK(!context.HOME.empty());
    CHECK(context.args.shell == "bash");
}

#endif
