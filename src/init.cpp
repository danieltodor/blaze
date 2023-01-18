#include "init.hpp"
#include "init/bash.hpp"

void init_shell(Context context)
{
    if (context.shell == "bash")
    {
        init_bash(context.config);
    }
}
