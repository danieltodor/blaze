_blaze_get_current_time() {
    echo "$(date +%s.%N)"
}

_blaze_preexec() {
    _blaze_start_time=$(_blaze_get_current_time)
    _blaze_first_print=false
    _blaze_previous_command=$1
    _blaze_previous_status=$_blaze_status
    blaze zsh --transient-prompt -e=$_blaze_previous_status -p=$_blaze_previous_command
}

_blaze_precmd() {
    local exit_code=$?
    _blaze_status=${exit_code:-0} # Default to 0 in case of ""
    _blaze_finish_time=$(_blaze_get_current_time)
}

TRAPINT() {
    echo # ^C needs an extra new line to prevent overwriting content during transient prompt
    _blaze_preexec
    return $(( 128 + $1 ))
}

_blaze_default_background=$(~/.local/share/blaze/util.bash _blaze_get_current_background)
_blaze_start_time=0
_blaze_finish_time=0
_blaze_status=0
_blaze_first_print=true
_blaze_previous_command=""
_blaze_previous_status=0

autoload -Uz add-zsh-hook
add-zsh-hook preexec _blaze_preexec # Executed just after a command has been read and is about to be executed
add-zsh-hook precmd _blaze_precmd # Executed before each prompt

setopt prompt_subst # Enable prompt substitution

ZLE_RPROMPT_INDENT="0" # Remove extra space at the end of right prompt
PROMPT='$(blaze zsh --prompt -s=$_blaze_start_time -f=$_blaze_finish_time -e=$_blaze_status -b=$_blaze_default_background -g=$_blaze_first_print)'
RPROMPT='$(blaze zsh --right-prompt -s=$_blaze_start_time -f=$_blaze_finish_time -e=$_blaze_status -b=$_blaze_default_background -g=$_blaze_first_print)'
