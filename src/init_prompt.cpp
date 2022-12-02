#include <iostream>
#include <string>

void init_bash()
{
    std::string command = R"d(
save_start_time() {
    date +%s.%N > "/dev/shm/${USER}.bashtime.${ROOTPID}"
}

get_start_time() {
    echo "$(cat /dev/shm/${USER}.bashtime.${ROOTPID})"
}

get_finish_time() {
    echo "$(date +%s.%N)"
}

run_on_exit() {
    rm /dev/shm/${USER}.bashtime.${ROOTPID}
}
trap run_on_exit EXIT

ROOTPID=$BASHPID
save_start_time
    )d";
    std::cout << command << '\n';
}

void set_prompt_command()
{
    std::cout << "PROMPT_COMMAND=\"echo\"\n";
}

void set_ps0()
{
    std::cout << "PS0='$(save_start_time)'\n";
}

void set_ps1()
{
    std::cout << "PS1='$(~/Documents/Programming/blaze/build/bin/blaze $(get_start_time) $(get_finish_time))'\n";
}

void init_prompt()
{
    init_bash();
    set_prompt_command();
    set_ps0();
    set_ps1();
}
