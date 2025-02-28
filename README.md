# Blaze
**A customizable and informative prompt for bash, fish, zsh, on linux and macOS.**

**Features: easily configurable, transient prompt, git integration, command execution time,
exit status, user defined modules, and more.**

![](media/demo.png)

[Installation](#installation)\
[Configuration](#configuration)\
[Builtin modules](#builtin-modules)\
[Creating your own modules](#creating-your-own-modules)\
[Update](#update)\
[Removal](#removal)

## Installation
1. Download and install blaze\
    Requirements: `git`, `make`, `g++`
    ``` sh
    git clone https://github.com/danieltodor/blaze.git &&
    cd blaze &&
    make install release=y
    ```

2. Setup your shell\
    Make sure `~/.local/bin` is added to PATH.
    - Bash
        ``` sh
        echo 'eval "$(blaze bash --init)"' >> ~/.bashrc
        ```
    - Fish\
        Depending on your config, you should move the added line where only interactive shells will use it.
        ``` sh
        echo 'blaze fish --init | source' >> ~/.config/fish/config.fish
        ```
    - Zsh
        ``` sh
        echo 'eval "$(blaze zsh --init)"' >> ~/.zshrc
        ```

3. Configure\
Open a new shell instance, and select a configuration with the `./use_config <name>` command.\
For additional configuration options, check out the [configuration](#configuration) section.\
It is recommended to use a [nerd font](https://www.nerdfonts.com) in your terminal emulator,
so that the symbols are displayed correctly.\
Note that some of the configs are suitable only for shells with right prompt.

## Configuration
Configuration files are read from these locations in order:
1. `BLAZE_CONFIG` env variable
2. `~/.config/blaze/config.toml`

``` toml
# For color values you can use named values like "black" "red" "green" "yellow" "blue" "magenta" "cyan" "white".
# If your terminal emulator supports the aixterm specification, you can also use these colors with the "bright_" prefix.
# You can use color ids from "0" to "255". https://www.ditig.com/256-colors-cheat-sheet
# If your terminal emulator has truecolor support, you can also use RGB and hexadecimal colors like "145, 255, 0" or "#3dff59".
# Special values:
#   - "default" The background color of the (xterm compatible) terminal emulator.

[global]
# This will be added before and after the string in each module
padding = ""

[prompt]
# Prompt string
string = " ❯ "
# Foreground color to use
foreground = ""
# Foreground color to use when the last command was exited with non zero code. Use only if different from foreground
error_foreground = ""
# Separate prompts with additional new line
new_line = false
# Create a separator line between prompts from this character
separator = ""
# Modules are displayed only for the last prompt
transient = false
# Start prompt on the bottom of the terminal window
bottom = false

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
# Name of a builtin module
name = ""
# Execute a user defined module
execute = ""
# Optional content for certain modules
content = ""
# Which level the module should be displayed on (required only for multilevel prompt)
level = 1
# The position of the module (if omitted, modules will be displayed as they appear in config)
position = 1
# Display it on left/right side. Or display it as right_prompt if the shell supports it
align = "left"
# Same as global one, but takes precedence over it
padding = ""
# Displayed inside the module, before the content (foreground used as color)
inner_prefix = ""
# Displayed inside the module, after the content (foreground used as color)
inner_suffix = ""
# Displayed as the beginning of the module (background used as color)
outer_prefix = ""
# Displayed as the end of the module (background used as color)
outer_suffix = ""
# Foreground color to use
foreground = ""
# Background color to use
background = ""
# Text styling to use
bold = false
dim = false
italic = false
underline = false

[directory]
# Maximum number of parent folders to display. Use 0 to disable truncation
limit = 10
# Show folders from the root of the repository
from_repository = true

[duration]
# Display module if execution time exceeds threshold
threshold = 2.0
# Digits displayed after seconds
precision = 1
# Display franctional part if less than this
fractional_until = 10.0

[status]
# Show exit status when it is 0 (success)
show_zero = false

[git_branch]
# Show branch name even when the HEAD is in a detached state
show_when_detached = false
# Ignore branches that matches these regex patterns, e.g.: "^master"
ignore = []

[git_commit]
# Show commit hash even when the HEAD points to a branch
show_when_attached = false
# Commit hash length (might be longer, depending on uniqueness). Use 0 to disable trimming
length = 7

[git_status]
# Show the number of changes
count = true
# Spacing between the elements
spacing = " "
# Displayed when the repository is clean
clean = "✓"
# Displayed when the current branch is ahead of the remote branch
ahead = "↑"
# Displayed when the current branch is behind the remote branch
behind = "↓"
# Displayed when there are stashed files
stashed = "$"
# Displayed when there are untracked files
untracked = "?"
# Displayed when there are conflicted files
conflicted = "="
# Displayed when there are modified files
modified = "!"
# Displayed when there are staged files
staged = "+"
# Displayed when there are renamed files
renamed = "»"
# Displayed when there are deleted files
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
| fixed | Can be used to display fixed text |
| separator | Can be used for additional separation between modules |
| directory | Current working directory |
| duration | Execution time of the last command (wall time) |
| status | Exit status of the last command |
| git_branch | Active branch in the repository |
| git_commit | Commit hash |
| git_status | Symbols indicating the current state of the repository |
| date | Current date [(format options)](https://en.cppreference.com/w/cpp/chrono/c/strftime) |
| time | Current time [(format options)](https://en.cppreference.com/w/cpp/chrono/c/strftime) |
| user | Current user |
| host | Hostname |
| shell | Currently active shell |

## Creating your own modules
You can use the programming language of your choice to create a custom module.
1. Create a script/binary that writes something to stdout.
``` py
# Check the PWD and/or the file/directory contents, or whatever you need.
# At the end, write something to stdout.
# If stdout is empty at the end, the module and its prefix/suffix content won't be displayed.
# This way you can create modules that are only used in certain directories.
print('something')
```
2. Create a new module entry in your config file, and tell blaze how and where it can be executed.
``` toml
[[module]]
execute = "python path/to/file.py"
# Additional styling like background, foreground, etc
```

## Update
`cd` into the downloaded blaze directory and run this one-liner to update it.
``` sh
git pull && make clean && make install release=y
```
It is advised to also update the config either via the `./use_config <name>` command, or manually,
as sometimes there are breaking changes.

## Removal
1. Delete the setup line from your shell's rc file.
2. `cd` into the downloaded blaze directory and run `make uninstall`.
3. Delete the `config.toml` file if you created one.
