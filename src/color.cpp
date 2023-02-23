#include <unordered_map>

#include "color.hpp"
#include "util.hpp"

const std::string csi = "\033["; // Control Sequence Introducer
const std::string foreground_rgb_prefix = "38;2;";
const std::string background_rgb_prefix = "48;2;";
const std::string foreground_id_prefix = "38;5;";
const std::string background_id_prefix = "48;5;";
const std::unordered_map<std::string, std::string> sequence_begin = {
    {"bash", "\001"} // \[
};
const std::unordered_map<std::string, std::string> sequence_end = {
    {"bash", "\002"} // \]
};
const std::unordered_map<std::string, int> foreground_color_map = {
    {"black", BLACK},
    {"red", RED},
    {"green", GREEN},
    {"yellow", YELLOW},
    {"blue", BLUE},
    {"magenta", MAGENTA},
    {"cyan", CYAN},
    {"white", WHITE},
    {"bright_black", BRIGHT_BLACK},
    {"bright_red", BRIGHT_RED},
    {"bright_green", BRIGHT_GREEN},
    {"bright_yellow", BRIGHT_YELLOW},
    {"bright_blue", BRIGHT_BLUE},
    {"bright_magenta", BRIGHT_MAGENTA},
    {"bright_cyan", BRIGHT_CYAN},
    {"bright_white", BRIGHT_WHITE}
};

std::string create_sequence(const std::string &code, const Context &context)
{
    std::string result = "";
    result += sequence_begin.at(context.shell);
    result += csi;
    result += code;
    result += 'm';
    result += sequence_end.at(context.shell);
    return result;
}

std::string to_color_code(const std::string &color, const int offset, const Context &context)
{
    if (color.empty())
    {
        return "";
    }
    else if (foreground_color_map.find(color) != foreground_color_map.end())
    {
        const int code = foreground_color_map.at(color);
        return create_sequence(std::to_string(code + offset), context);
    }
    else if (is_number(color))
    {
        const std::string id_prefix = offset == 0 ? foreground_id_prefix : background_id_prefix;
        return create_sequence(id_prefix + color, context);
    }
    else
    {
        const std::string rgb_prefix = offset == 0 ? foreground_rgb_prefix : background_rgb_prefix;
        std::string rgb_color = color;
        if (color == "default")
        {
            rgb_color = context.DEFAULT_BACKGROUND;
        }
        return create_sequence(rgb_prefix + rgb_color, context);
    }
}

std::string set_text_mode(const int code, const Context &context)
{
    return create_sequence(std::to_string(code), context);
}

std::string set_foreground(const std::string &color, const Context &context)
{
    return to_color_code(color, 0, context);
}

std::string set_background(const std::string &color, const Context &context)
{
    return to_color_code(color, BACKGROUND_COLOR_OFFSET, context);
}

std::string reset_all(const Context &context)
{
    return create_sequence(std::to_string(RESET), context);
}

std::string reset_text_mode(const int code, const Context &context)
{
    int special_offset = 0;
    if (code == BOLD)
    {
        special_offset = 1;
    }
    return create_sequence(std::to_string(code + TEXT_TYPE_RESET_OFFSET + special_offset), context);
}

std::string reset_foreground(const Context &context)
{
    return create_sequence(std::to_string(DEFAULT_COLOR), context);
}

std::string reset_background(const Context &context)
{
    return create_sequence(std::to_string(DEFAULT_COLOR + BACKGROUND_COLOR_OFFSET), context);
}
