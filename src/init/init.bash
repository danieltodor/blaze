_blaze_get_current_time() {
    echo "$(date +%s.%N)"
}

_blaze_save_start_time() {
    echo $(_blaze_get_current_time) > $_blaze_time_file
}

_blaze_get_start_time() {
    echo "$(cat $_blaze_time_file)"
}

_blaze_save_exit_status() {
    echo $? > $_blaze_exit_status_file
}

_blaze_get_exit_status() {
    echo "$(cat $_blaze_exit_status_file)"
}

_blaze_run_on_exit() {
    rm $_blaze_time_file
    rm $_blaze_exit_status_file
}
trap _blaze_run_on_exit EXIT

_blaze_session="_blaze_${USER}_${BASHPID}"

if [[ -d "/dev/shm" ]]; then
    _blaze_file_prefix="/dev/shm/$_blaze_session"
else
    _blaze_file_prefix="/tmp/$_blaze_session"
fi

_blaze_default_background=$(~/.local/share/blaze/util.bash _blaze_get_current_background)
_blaze_time_file="${_blaze_file_prefix}_time"
_blaze_exit_status_file="${_blaze_file_prefix}_exit_status"

_blaze_save_start_time
_blaze_save_exit_status

# Bash doesn`t have preexec/precmd hooks, but the behavior of these prompt variables are close enough
PS0=$PS0'$(_blaze_save_start_time)' # Is expanded after a command is read and before the command is executed
PROMPT_COMMAND=$PROMPT_COMMAND'$(_blaze_save_exit_status)' # Executed before the printing of each primary prompt

PS1='$(blaze bash --prompt -s $(_blaze_get_start_time) -f $(_blaze_get_current_time) -e $(_blaze_get_exit_status) -b $_blaze_default_background)'
