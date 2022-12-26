#include <string>
#include <unordered_map>

#include "init_shell.hpp"
#include "draw_prompt.hpp"

#define MAX_ARG_COUNT 2

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return 0;
    }
    std::unordered_map<int, std::string *> args;
    std::string first, second;
    args[1] = &first;
    args[2] = &second;
    for (int i = 1; i < argc && i < MAX_ARG_COUNT + 1; i++)
    {
        *args[i] = argv[i];
    }

    if (first == "init")
    {
        init_shell(second);
    }
    else
    {
        double start_time = std::stod(first);
        double finish_time = std::stod(second);
        draw_prompt(start_time, finish_time);
    }
    return 0;
}
