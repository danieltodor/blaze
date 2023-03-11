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
    echo $(blaze_get_current_time) > $blaze_time_file
}

blaze_get_start_time() {
    echo "$(cat $blaze_time_file)"
}

blaze_run_on_exit() {
    rm $blaze_time_file
}
trap blaze_run_on_exit EXIT

blaze_default_background=$(blaze_get_current_background)
blaze_time_file_name=${USER}_bashtime_${BASHPID}

if [[ -d "/dev/shm" ]]; then
    blaze_time_file="/dev/shm/$blaze_time_file_name"
else
    blaze_time_file="/tmp/$blaze_time_file_name"
fi

blaze_save_start_time

PS0='$(blaze_save_start_time)'
PS1='$(blaze bash -s $(blaze_get_start_time) -f $(blaze_get_current_time) -b $blaze_default_background)'
