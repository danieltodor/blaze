#include "context.hpp"
#include "util.hpp"
#include "pool.hpp"

Context get_context(int argc, char *argv[])
{
    Args args;
    Config config;
    bool git_repository_detected = false;
    bool git_repository_detached = false;
    thread_pool.detach_task(
        [&args, argc, argv]
        {
            args = std::move(argparse::parse<Args>(argc, argv));
        }
    );
    thread_pool.detach_task(
        [&config]
        {
            config = get_config();
        }
    );
    thread_pool.detach_task(
        [&git_repository_detected, &git_repository_detached]
        {
            check_git_repository(git_repository_detected, git_repository_detached);
        }
    );
    thread_pool.wait();
    Context context = {
        args,
        config,
        get_env("HOME"),
        get_env("PWD"),
        git_repository_detected,
        git_repository_detached,
        get_current_time()
    };
    return context;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("get_context")
{
    const int argc = 2;
    const char *argv[] = {"name", "bash"};
    Context context = get_context(argc, const_cast<char**>(argv));
    CHECK(!context.HOME.empty());
    CHECK(context.args.shell == "bash");
}

#endif
