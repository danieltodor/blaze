#include <string>

#include "init_prompt.hpp"
#include "draw_prompt.hpp"

int main(int argc, char *argv[])
{
    (void)argc;
    std::string first = argv[1];
    std::string second = argv[2];
    if (first == "init")
    {
        init_prompt();
        return 0;
    }
    double start_time = std::stod(first);
    double finish_time = std::stod(second);
    draw_prompt(start_time, finish_time);
    return 0;
}
