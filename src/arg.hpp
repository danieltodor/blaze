#pragma once

#include <string>

#include "external/argparse/argparse.hpp"

struct Args : public argparse::Args
{
    Args() = default;

    ~Args() = default;

    Args(const Args &obj) = default;

    Args & operator = (Args &&obj)
    {
        shell = obj.shell;
        init = obj.init;
        prompt = obj.prompt;
        right_prompt = obj.right_prompt;
        transient_prompt = obj.transient_prompt;
        first_print = obj.first_print;
        start_time = obj.start_time;
        finish_time = obj.finish_time;
        status = obj.status;
        background = obj.background;
        previous_command = obj.previous_command;
        return *this;
    }

    std::string &shell = arg("Current shell");
    bool &init = flag("init", "Init current shell").set_default(false);
    bool &prompt = flag("prompt", "Print prompt").set_default(false);
    bool &right_prompt = flag("right-prompt", "Print right prompt").set_default(false);
    bool &transient_prompt = flag("transient-prompt", "Transient prompt").set_default(false);
    bool &first_print = kwarg("g,first-print", "First print of prompt").set_default(false);
    double &start_time = kwarg("s,start-time", "Time when the command was started").set_default(0);
    double &finish_time = kwarg("f,finish-time", "Time when the command was finished").set_default(0);
    int &status = kwarg("e,status", "Exit status of the last command").set_default(0);
    std::string &background = kwarg("b,background", "Background RGB color").set_default("0;0;0");
    std::string &previous_command = kwarg("p,previous-command", "Previous command").set_default("");
};
