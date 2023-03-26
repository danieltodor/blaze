blaze_get_current_time() {
    echo "$(date +%s.%N)"
}

blaze_preexec() {
    blaze_start_time=$(blaze_get_current_time)
}

blaze_precmd() {
    blaze_exit_status=$?
    blaze_finish_time=$(blaze_get_current_time)
}

blaze_default_background=$(~/.local/share/blaze/util.bash get_current_background)
blaze_start_time=""
blaze_finish_time=""
blaze_exit_status=""

blaze_preexec
blaze_precmd

autoload -Uz add-zsh-hook
add-zsh-hook preexec blaze_preexec
add-zsh-hook precmd blaze_precmd

setopt prompt_subst # Enable prompt substitution

ZLE_RPROMPT_INDENT="0" # Remove extra space at the end of right prompt
PROMPT='$(blaze zsh --prompt -s $blaze_start_time -f $blaze_finish_time -e $blaze_exit_status -b $blaze_default_background)'
RPROMPT='$(blaze zsh --right-prompt -s $blaze_start_time -f $blaze_finish_time -e $blaze_exit_status -b $blaze_default_background)'
