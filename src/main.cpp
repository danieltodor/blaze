#include <string>

#include "init.hpp"
#include "print.hpp"
#include "external/argparse.hpp"

struct blaze_args : public argparse::Args
{
    std::string &init = kwarg("i,init", "Which shell to init").set_default("");
    std::string &start_time = kwarg("s,start_time", "Time when the command was started").set_default("");
    std::string &finish_time = kwarg("f,finish_time", "Time when the command was finished").set_default("");
};

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return 0;
    }
    blaze_args args = argparse::parse<blaze_args>(argc, argv);
    if (!args.init.empty())
    {
        init_shell(args.init);
    }
    else
    {
        double start_time = std::stod(args.start_time);
        double finish_time = std::stod(args.finish_time);
        print_all(start_time, finish_time);
    }
    return 0;
}
