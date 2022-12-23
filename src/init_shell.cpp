#include "init_shell.hpp"
#include "init/init_bash.hpp"

void init_shell(std::string shell)
{
    if (shell == "bash")
    {
        init_bash();
    }
}
