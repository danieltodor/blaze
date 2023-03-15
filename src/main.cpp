#include <string>

#include "external/argparse/argparse.hpp"
#include "src/init.hpp"
#include "src/print.hpp"
#include "src/context.hpp"
#include "src/config.hpp"
#include "src/util.hpp"

struct Args : public argparse::Args
{
    std::string &shell = arg("Current shell");
    bool &init = flag("i,init", "Init current shell");
    std::string &start_time = kwarg("s,start-time", "Time when the command was started").set_default("0");
    std::string &finish_time = kwarg("f,finish-time", "Time when the command was finished").set_default("0");
    std::string &exit_status = kwarg("e,exit-status", "Exit status of the last command").set_default("0");
    std::string &background = kwarg("b,background", "Background RGB color").set_default("0;0;0");
};

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return 0;
    }
    const Args args = argparse::parse<Args>(argc, argv);
    Context context = {
        get_config(),
        args.shell,
        std::stod(args.start_time),
        std::stod(args.finish_time),
        args.exit_status,
        args.background,
        get_env("PWD"),
        check_git_repository(),
        get_current_time()
    };
    if (args.init)
    {
        init_shell(context);
    }
    else
    {
        print_all(context);
    }
    return 0;
}
