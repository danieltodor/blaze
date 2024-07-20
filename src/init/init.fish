function _blaze_get_current_time
    echo (date +%s.%N)
end

function _blaze_preexec --on-event fish_preexec
    set _blaze_start_time (_blaze_get_current_time)
end

function _blaze_precmd --on-event fish_prompt
    set _blaze_exit_status $status
    set _blaze_finish_time (_blaze_get_current_time)
end

set _blaze_default_background (~/.local/share/blaze/util.bash _blaze_get_current_background)
set _blaze_start_time ""
set _blaze_finish_time ""
set _blaze_exit_status ""

_blaze_preexec
_blaze_precmd

function fish_prompt
    blaze fish --prompt -s $_blaze_start_time -f $_blaze_finish_time -e $_blaze_exit_status -b $_blaze_default_background
end

function fish_right_prompt
    blaze fish --right-prompt -s $_blaze_start_time -f $_blaze_finish_time -e $_blaze_exit_status -b $_blaze_default_background
end
