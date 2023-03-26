#include "src/init.hpp"
#include "src/print.hpp"
#include "src/context.hpp"
#include "src/util.hpp"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return 0;
    }
    Context context = {
        argparse::parse<Args>(argc, argv),
        get_config(),
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
