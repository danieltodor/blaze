#include <string>
#include <unordered_map>

#include "init_shell.hpp"
#include "draw_prompt.hpp"

int main(int argc, char *argv[])
{
    std::unordered_map<int, std::string *> args;
    std::string first, second, third;
    args[1] = &first;
    args[2] = &second;
    args[3] = &third;
    for (int i = 1; i < argc; i++)
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
