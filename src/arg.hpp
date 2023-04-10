#ifndef BLAZE_ARG_HPP_INCLUDED
#define BLAZE_ARG_HPP_INCLUDED

#include <string>

#include "external/argparse/argparse.hpp"

struct Args : public argparse::Args
{
    std::string &shell = arg("Current shell");
    bool &init = flag("i,init", "Init current shell").set_default(false);
    bool &prompt = flag("p,prompt", "Print prompt").set_default(false);
    bool &right_prompt = flag("r,right-prompt", "Print right prompt").set_default(false);
    std::string &start_time = kwarg("s,start-time", "Time when the command was started").set_default("0");
    std::string &finish_time = kwarg("f,finish-time", "Time when the command was finished").set_default("0");
    std::string &exit_status = kwarg("e,exit-status", "Exit status of the last command").set_default("0");
    std::string &background = kwarg("b,background", "Background RGB color").set_default("0;0;0");
};

#endif
