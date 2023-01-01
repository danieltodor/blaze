#include "init.hpp"
#include "init/bash.hpp"

void init_shell(std::string shell)
{
    if (shell == "bash")
    {
        init_bash();
    }
}
