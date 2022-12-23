#include <string>
#include <unordered_map>

#include "init_shell.hpp"
#include "draw_prompt.hpp"

#define MAX_ARG_COUNT 3

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return 0;
    }
    std::unordered_map<int, std::string *> args;
    std::string first, second, third;
    args[1] = &first;
    args[2] = &second;
    args[3] = &third;
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
        double start_time = std::stod(second);
        double finish_time = std::stod(third);
        draw_prompt(first, start_time, finish_time);
    }
    return 0;
}
