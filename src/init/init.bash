_blaze_get_current_time() {
    echo "$(date +%s.%N)"
}

_blaze_save_start_time() {
    echo $(_blaze_get_current_time) > $_blaze_start_time_file
}

_blaze_get_start_time() {
    echo "$(cat $_blaze_start_time_file)"
}

_blaze_save_status() {
    local exit_code=$?
    echo ${exit_code:-0} > $_blaze_status_file # Default to 0 in case of ""
}

_blaze_get_status() {
    echo "$(cat $_blaze_status_file)"
}

_blaze_save_first_print() {
    if [[ -e $_blaze_first_print_file ]]; then
        echo "false" > $_blaze_first_print_file
    else
        echo "true" > $_blaze_first_print_file
    fi
}

_blaze_get_first_print() {
    echo "$(cat $_blaze_first_print_file)"
}

_blaze_save_previous_command() {
    echo "$(fc -ln -1 | sed 's/^[[:space:]]*//')" > $_blaze_previous_command_file
}

_blaze_get_previous_command() {
    echo "$(cat $_blaze_previous_command_file)"
}

_blaze_save_previous_status() {
    echo $(_blaze_get_status) > $_blaze_previous_status_file
}

_blaze_get_previous_status() {
    echo "$(cat $_blaze_previous_status_file)"
}

_blaze_preexec() {
    _blaze_save_start_time
    _blaze_save_first_print
    _blaze_save_previous_command
    _blaze_save_previous_status
    blaze bash --transient-prompt -e=$(_blaze_get_previous_status) -p="$(_blaze_get_previous_command)"
}

_blaze_precmd() {
    _blaze_save_status
}

_blaze_run_on_exit() {
    rm $_blaze_start_time_file
    rm $_blaze_status_file
    rm $_blaze_first_print_file
    rm $_blaze_previous_command_file
    rm $_blaze_previous_status_file
}
trap _blaze_run_on_exit EXIT
trap "echo; _blaze_preexec" SIGINT # ^C needs an extra new line to prevent overwriting content during transient prompt

_blaze_session="_blaze_${USER}_${BASHPID}"

if [[ -d "/dev/shm" ]]; then
    _blaze_file_prefix="/dev/shm/$_blaze_session"
else
    _blaze_file_prefix="/tmp/$_blaze_session"
fi

_blaze_default_background=$(~/.local/share/blaze/util.bash _blaze_get_current_background)
_blaze_start_time_file="${_blaze_file_prefix}_start_time"
_blaze_status_file="${_blaze_file_prefix}_status"
_blaze_first_print_file="${_blaze_file_prefix}_first_print"
_blaze_previous_command_file="${_blaze_file_prefix}_previous_command"
_blaze_previous_status_file="${_blaze_file_prefix}_previous_status"

_blaze_save_start_time
_blaze_save_status
_blaze_save_first_print
_blaze_save_previous_command
_blaze_save_previous_status

# Bash doesn't have preexec/precmd hooks, but the behavior of these prompt variables are close enough
PS0=$PS0'$(_blaze_preexec)' # Is expanded after a command is read and before the command is executed
PROMPT_COMMAND=$PROMPT_COMMAND'$(_blaze_precmd)' # Executed before the printing of each primary prompt

PS1='$(blaze bash --prompt -s=$(_blaze_get_start_time) -f=$(_blaze_get_current_time) -e=$(_blaze_get_status) -b=$_blaze_default_background -g=$(_blaze_get_first_print))'
