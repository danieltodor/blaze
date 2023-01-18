#ifndef BLAZE_INIT_BASH_HPP_INCLUDED
#define BLAZE_INIT_BASH_HPP_INCLUDED

#include <iostream>
#include <string>

#include "src/config.hpp"

void init_bash(Config config)
{
    std::cout << R"d(
blaze_get_current_time() {
    echo "$(date +%s.%N)"
}

blaze_save_start_time() {
    echo $(blaze_get_current_time) > $BLAZE_TIME_FILE
}

blaze_get_start_time() {
    echo "$(cat $BLAZE_TIME_FILE)"
}

blaze_run_on_exit() {
    rm $BLAZE_TIME_FILE
}
trap blaze_run_on_exit EXIT

BLAZE_TIME_FILE_NAME=${USER}_bashtime_${BASHPID}

if [[ -d "/dev/shm" ]]; then
    BLAZE_TIME_FILE="/dev/shm/$BLAZE_TIME_FILE_NAME"
else
    BLAZE_TIME_FILE="/tmp/$BLAZE_TIME_FILE_NAME"
fi

blaze_save_start_time

PS0='$(blaze_save_start_time)'
PS1='$(blaze -c bash -s $(blaze_get_start_time) -f $(blaze_get_current_time))'
)d";
    std::string new_line = config.global.new_line ? "echo" : "";
    printf("PROMPT_COMMAND=\"%s\"", new_line.c_str());
}

#endif
