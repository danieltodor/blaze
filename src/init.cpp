#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "init.hpp"
#include "util.hpp"

void init_shell(const Context &context)
{
    const std::string file_name = "init." + context.shell;
    const std::string paths[] = {
        context.PWD + "/src/init/" + file_name, // TODO: This can be removed (?)
        get_env("HOME") + "/.local/share/blaze/" + file_name
    };
    std::ifstream file;
    for (const std::string &path : paths)
    {
        file.open(path);
        if (file.is_open())
        {
            break;
        }
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << buffer.str();
}
