blaze_get_current_background() {
    exec < /dev/tty
    oldstty=$(stty -g)
    stty raw -echo min 0
    col=11 # background
    echo -en "\033]${col};?\033\\" > /dev/tty
    IFS=';' read -r -d '\' hex_rgb
    stty $oldstty
    hex_rgb=$(echo $hex_rgb | sed 's/^.*\;//;s/[^rgb:0-9a-f/]//g')
    hex_rgb=$(echo $hex_rgb | sed 's/[rgb:|/]//g')
    bg_rgb=
    bg_rgb+=$((16#${hex_rgb:0:4} >> 8))";"
    bg_rgb+=$((16#${hex_rgb:4:4} >> 8))";"
    bg_rgb+=$((16#${hex_rgb:8:4} >> 8))
    echo $bg_rgb
}

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

export DEFAULT_BACKGROUND=$(blaze_get_current_background)

PS0='$(blaze_save_start_time)'
PS1='$(blaze bash -s $(blaze_get_start_time) -f $(blaze_get_current_time))'
