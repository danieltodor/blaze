# Get the RGB color of the terminal emulator. Works only in xterm compatible emulators.
# FIXME: stty command not working. Find another way.
function blaze_get_current_background
    echo '0;0;0'
    # set -l ps 11 # background
    # echo -en "\033]"$ps";?\033\\"
    # set IFS ';' read -l -d \ hex_rgb
    # set -l hex_rgb (string replace --regex ']11;rgb:|/' '' $hex_rgb)
    # set -l bg_rgb ""
    # # Convert hex parts to decimal, and shift them to fit into range of 0-255
    # set -l h1 (string sub -s 1 -l 4 $hex_rgb)
    # set -l h2 (string sub -s 5 -l 4 $hex_rgb)
    # set -l h3 (string sub -s 9 -l 4 $hex_rgb)
    # set -a bg_rgb (echo "print(int('$h1', 16) >> 8)" | python)
    # set -a bg_rgb ";"
    # set -a bg_rgb (echo "print(int('$h2', 16) >> 8)" | python)
    # set -a bg_rgb ";"
    # set -a bg_rgb (echo "print(int('$h3', 16) >> 8)" | python)
    # set bg_rgb (echo "print('$bg_rgb'.replace(' ', ''))" | python)
    # echo $bg_rgb
end

function blaze_get_current_time
    echo (date +%s.%N)
end

function blaze_preexec --on-event fish_preexec
    set blaze_start_time (blaze_get_current_time)
end

function blaze_precmd --on-event fish_prompt
    set blaze_exit_status $status
    set blaze_finish_time (blaze_get_current_time)
end

set blaze_default_background (blaze_get_current_background)
set blaze_start_time ""
set blaze_finish_time ""
set blaze_exit_status ""

blaze_preexec
blaze_precmd

function fish_prompt
    blaze fish -s $blaze_start_time -f $blaze_finish_time -e $blaze_exit_status -b $blaze_default_background
end
