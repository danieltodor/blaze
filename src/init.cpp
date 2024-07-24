#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "src/init.hpp"
#include "src/util.hpp"

// Prepare shell init string
std::string prepare_shell_init(const Context &context)
{
    const std::string file_name = "init." + context.args.shell;
    const std::string paths[] = {
        // context.PWD + "/src/init/" + file_name, // For development
        context.HOME + "/.local/share/blaze/" + file_name
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
    return buffer.str();
}

void print_shell_init(const Context &context)
{
    std::cout << prepare_shell_init(context);
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("prepare_shell_init")
{
    Context context;
    context.args.shell = "bash";
    context.HOME = get_env("HOME");
    const std::string result = prepare_shell_init(context);
    CHECK(result.find("PS1='") != std::string::npos);
}

#endif
