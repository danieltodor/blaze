#include <string>

#include "init.hpp"
#include "print.hpp"
#include "context.hpp"
#include "external/argparse/argparse.hpp"

struct Args : public argparse::Args
{
    std::string &init = kwarg("i,init", "Which shell to init").set_default("");
    std::string &current_shell = kwarg("c,current_shell", "Current shell").set_default("");
    std::string &start_time = kwarg("s,start_time", "Time when the command was started").set_default("0");
    std::string &finish_time = kwarg("f,finish_time", "Time when the command was finished").set_default("0");
};

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return 0;
    }
    Args args = argparse::parse<Args>(argc, argv);
    Context context = {
        Config(),
        !args.init.empty() ? args.init : args.current_shell,
        std::stod(args.start_time),
        std::stod(args.finish_time)
    };
    if (!args.init.empty())
    {
        init_shell(context);
    }
    else
    {
        print_all(context);
    }
    return 0;
}
