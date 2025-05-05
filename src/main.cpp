#include "init.hpp"
#include "print.hpp"
#include "context.hpp"
#include "test.hpp"

int main(int argc, char *argv[])
{
#ifdef TEST
    run_tests(argc, argv);
#endif
    Context context = get_context(argc, argv);
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
    else if (context.args.transient_prompt)
    {
        print_transient_prompt(context);
    }
    return 0;
}
