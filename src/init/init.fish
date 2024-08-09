function _blaze_get_current_time
    echo (date +%s.%N)
end

function _blaze_preexec --on-event fish_preexec # Executed just after a command has been read and is about to be executed
    set _blaze_start_time (_blaze_get_current_time)
    set _blaze_first_print false
    set _blaze_previous_command $argv
    set _blaze_previous_status $_blaze_status
    blaze fish --transient-prompt -e=$_blaze_previous_status -p=$_blaze_previous_command
end

function _blaze_precmd --on-event fish_prompt # Executed before each prompt
    set _blaze_status $status
    set _blaze_finish_time (_blaze_get_current_time)
end

set _blaze_default_background (~/.local/share/blaze/util.bash _blaze_get_current_background)
set _blaze_start_time 0
set _blaze_finish_time 0
set _blaze_status 0
set _blaze_first_print true
set _blaze_previous_command ""
set _blaze_previous_status 0

function fish_prompt
    blaze fish --prompt -s=$_blaze_start_time -f=$_blaze_finish_time -e=$_blaze_status -b=$_blaze_default_background -g=$_blaze_first_print
end

function fish_right_prompt
    blaze fish --right-prompt -s=$_blaze_start_time -f=$_blaze_finish_time -e=$_blaze_status -b=$_blaze_default_background -g=$_blaze_first_print
end
