#include "src/init.hpp"
#include "src/print.hpp"
#include "src/context.hpp"
#include "src/util.hpp"
#include "src/test.hpp"

int main(int argc, char *argv[])
{
#ifdef TEST
    run_tests(argc, argv);
#endif
    Context context = {
        argparse::parse<Args>(argc, argv),
        get_config(),
        get_env("HOME"),
        get_env("PWD"),
        check_git_repository(),
        get_current_time()
    };
    if (context.args.init)
    {
        print_shell_init(context);
    }
    else if (context.args.prompt)
    {
        print_prompt(context);
    }
    else if (context.args.right_prompt)
    {
        print_right_prompt(context);
    }
    return 0;
}
