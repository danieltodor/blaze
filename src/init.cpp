#include "init.hpp"
#include "init/bash.hpp"

void init_shell(const Context &context)
{
    if (context.shell == "bash")
    {
        init_bash(context.config);
    }
}
