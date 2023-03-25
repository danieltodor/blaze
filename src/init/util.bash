# Get the RGB color of the terminal emulator. Works only in xterm compatible emulators.
get_current_background() {
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

if [[ $# -eq 0 ]]; then
    echo "Pass the function name to call"
    exit 1
fi

# Call the function and echo the result
echo $($1)
