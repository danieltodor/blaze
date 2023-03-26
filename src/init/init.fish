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

set blaze_default_background (~/.local/share/blaze/util.bash get_current_background)
set blaze_start_time ""
set blaze_finish_time ""
set blaze_exit_status ""

blaze_preexec
blaze_precmd

function fish_prompt
    blaze fish --prompt -s $blaze_start_time -f $blaze_finish_time -e $blaze_exit_status -b $blaze_default_background
end

function fish_right_prompt
    blaze fish --rprompt -s $blaze_start_time -f $blaze_finish_time -e $blaze_exit_status -b $blaze_default_background
end
