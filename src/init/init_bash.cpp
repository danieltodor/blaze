#include <iostream>

#include "init_bash.hpp"

void init_bash()
{
    std::cout << R"d(
save_start_time() {
    date +%s.%N > "/dev/shm/${USER}.bashtime.${SESSION}"
}

get_start_time() {
    echo "$(cat /dev/shm/${USER}.bashtime.${SESSION})"
}

get_finish_time() {
    echo "$(date +%s.%N)"
}

run_on_exit() {
    rm /dev/shm/${USER}.bashtime.${SESSION}
}
trap run_on_exit EXIT

SESSION=$BASHPID
save_start_time

PROMPT_COMMAND="echo"
PS0='$(save_start_time)'
PS1='$(blaze bash $(get_start_time) $(get_finish_time))'
    )d";
}
