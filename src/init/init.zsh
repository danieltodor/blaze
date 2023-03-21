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

blaze_save_start_time() {
    echo $(blaze_get_current_time) > $blaze_time_file
}

blaze_get_start_time() {
    echo "$(cat $blaze_time_file)"
}

blaze_save_exit_status() {
    echo $? > $blaze_exit_status_file
}

blaze_get_exit_status() {
    echo "$(cat $blaze_exit_status_file)"
}

# TODO: Create a similar trap for zsh
# TRAPEXIT() {
#     rm $blaze_time_file
#     rm $blaze_exit_status_file
#     return $(( 128 + $1 ))
# }

blaze_session="${USER}_${PPID}"

if [[ -d "/dev/shm" ]]; then
    blaze_file_prefix="/dev/shm/$blaze_session"
else
    blaze_file_prefix="/tmp/$blaze_session"
fi

blaze_default_background=$(blaze_get_current_background)
blaze_time_file="${blaze_file_prefix}_time"
blaze_exit_status_file="${blaze_file_prefix}_exit_status"

autoload -Uz add-zsh-hook
add-zsh-hook preexec blaze_save_start_time
add-zsh-hook precmd blaze_save_exit_status

setopt prompt_subst # Enable prompt substitution

PROMPT='$(blaze zsh -s $(blaze_get_start_time) -f $(blaze_get_current_time) -e $(blaze_get_exit_status) -b $blaze_default_background)'
