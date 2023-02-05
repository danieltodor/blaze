# Blaze
- Icon
- Short info about the program
- Core features
- Picture about example prompts
- Example configs with pictures in an example dir

[Installation](#installation)\
[Configuration](#configuration)\
[Builtin modules](#builtin-modules)\
[Removal](#removal)

## Installation

1. Download and install blaze
    ``` sh
    git clone --depth=1 https://github.com/danieltodor/blaze.git
    cd blaze
    make
    make install
    ```

2. Setup your shell
    - Bash
        ``` sh
        echo 'eval "$(blaze --init bash)"' >> ~/.bashrc
        ```

3. Configure\
Open a new shell instance to see the new prompt.\
The default prompt is minimalistic. It contains only the working directory
and the last command`s execution time (if it exceeds the default treshold).\
If you want to modify it, check the [configuration](#configuration) section.

## Configuration

Configuration files are read from these locations in order:
1. `BLAZE_CONFIG` env variable
2. `~/.config/blaze.toml`
3. `/etc/blaze.toml`

``` toml
# For color values you can use named values like "black" "red" "green" "yellow" "blue" "magenta" "cyan" "white"
# or an RGB value like "145;255;0"

[global]
# Whether to add a new line before the prompt is rendered
new_line = false
# This will be added before and after the string in each module
padding = " "

[prompt]
# This is your PS1
string = " "
# Foreground color to use
foreground = ""

[connector]
# This character will be used as spacing between the left and right side modules
character = " "
# Foreground color to use
foreground = ""
# Background color to use
background = ""
# Text styling to use
dim = false

[[module]]
# Name of a builtin module.
name = ""
# Execute a user defined module
execute = ""
# Which level the module should be displayed on
level = 1
# The position of the module
position = 1
# Display it on left/right side
align = "left"
# Same as global one, but takes precedence over it.
padding = ""
# Displayed inside the module, before the content
inner_prefix = "";
# Displayed inside the module, after the content
inner_suffix = "";
# Displayed as the beginning of the module
outer_prefix = "";
# Displayed as the end of the module
outer_suffix = "";
# Foreground color to use
foreground = ""
# Background color to use
background = ""
# Text styling to use
bold = false
dim = false
italic = false
underline = false

[current_directory]
# Display the basename only
basename_only = false

[execution_time]
# Digits displayed after seconds
precision = 1
# Display module if execution time exceeds this
display_from = 0
# Display franctional part if less than this
display_fractional_until = 10

[git_branch]
# Ignore branches that matches these regex patterns, e.g.: "^master"
ignore = []

[git_status]
clean = ""
conflicted = "="
ahead = "⇡"
behind = "⇣"
diverged = "⇕"
untracked = "?"
stashed = "$"
modified = "!"
staged = "+"
renamed = "»"
deleted = "✘"

[date]
# Date format to use
format = "%x"

[time]
# Time format to use
format = "%X"
```

## Builtin modules

| Name | Description |
| - | - |
| separator | Can be used for additional separation between modules |
| directory | Current working directory |
| execution_time | Execution time of the last command (wall time) |
| git_branch | Active branch in the repository |
| git_status | Symbols indicating the current state of the repository |
| date | Current date |
| time | Current time |

## Creating your own modules
You can use any backend language to create a custom module.
1. Create a script/binary that prints something to stdout.
``` py
# Check the PWD and the file/dir contents, or do whatever you need to do.
# At the end, print the information you need.
print('important stuff')
```
2. Create a new module entry in your config file, and tell blaze how and where it can be executed.
``` toml
[[module]]
execute = "python path/to/file.py"
# Additional styling like background, foreground, etc
```

## Removal

1. Delete the setup line from your shell\`s rc file.
2. cd into the downloaded blaze directory and run `make uninstall`.
3. Delete the `blaze.toml` file if you created one.
