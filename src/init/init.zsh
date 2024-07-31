_blaze_get_current_time() {
    echo "$(date +%s.%N)"
}

_blaze_preexec() {
    _blaze_start_time=$(_blaze_get_current_time)
    _blaze_first_print=false
}

_blaze_precmd() {
    _blaze_exit_status=$?
    _blaze_finish_time=$(_blaze_get_current_time)
}

_blaze_default_background=$(~/.local/share/blaze/util.bash _blaze_get_current_background)
_blaze_start_time=$(_blaze_get_current_time)
_blaze_finish_time=$(_blaze_get_current_time)
_blaze_exit_status=$?
_blaze_first_print=true

autoload -Uz add-zsh-hook
add-zsh-hook preexec _blaze_preexec # Executed just after a command has been read and is about to be executed
add-zsh-hook precmd _blaze_precmd # Executed before each prompt

setopt prompt_subst # Enable prompt substitution

ZLE_RPROMPT_INDENT="0" # Remove extra space at the end of right prompt
PROMPT='$(blaze zsh --prompt -s $_blaze_start_time -f $_blaze_finish_time -e $_blaze_exit_status -b $_blaze_default_background -g $_blaze_first_print)'
RPROMPT='$(blaze zsh --right-prompt -s $_blaze_start_time -f $_blaze_finish_time -e $_blaze_exit_status -b $_blaze_default_background -g $_blaze_first_print)'
