# Get the RGB color of the terminal emulator. Works only in xterm compatible emulators.
blaze_get_current_background() {
    exec < /dev/tty
    local oldstty=$(stty -g)
    stty raw -echo min 0
    local ps=11 # background
    echo -en "\033]${ps};?\033\\" > /dev/tty
    local hex_rgb=""
    IFS=';' read -r -d '\' hex_rgb
    stty $oldstty
    hex_rgb=$(echo $hex_rgb | sed 's/^.*\;//;s/[^rgb:0-9a-f/]//g')
    hex_rgb=$(echo $hex_rgb | sed 's/[rgb:|/]//g')
    local bg_rgb=""
    # Convert hex parts to decimal, and shift them to fit into range of 0-255
    bg_rgb+=$((16#${hex_rgb:0:4} >> 8))";"
    bg_rgb+=$((16#${hex_rgb:4:4} >> 8))";"
    bg_rgb+=$((16#${hex_rgb:8:4} >> 8))
    echo $bg_rgb
}

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

blaze_default_background=$(blaze_get_current_background)
blaze_start_time=""
blaze_finish_time=""
blaze_exit_status=""

blaze_preexec
blaze_precmd

autoload -Uz add-zsh-hook
add-zsh-hook preexec blaze_preexec
add-zsh-hook precmd blaze_precmd

setopt prompt_subst # Enable prompt substitution

PROMPT='$(blaze zsh -s $blaze_start_time -f $blaze_finish_time -e $blaze_exit_status -b $blaze_default_background)'
