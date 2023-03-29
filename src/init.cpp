#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "src/init.hpp"
#include "src/util.hpp"

void print_shell_init(const Context &context)
{
    const std::string file_name = "init." + context.args.shell;
    const std::string paths[] = {
        // context.PWD + "/src/init/" + file_name,
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
