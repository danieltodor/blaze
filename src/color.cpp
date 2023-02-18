#include <unordered_map>

#include "color.hpp"

const std::string csi = "\033["; // Control Sequence Introducer
const std::string foreground_rgb_prefix = "38;2;";
const std::string background_rgb_prefix = "48;2;";
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
    {"white", WHITE}
};

std::string create_sequence(const std::string &code, const std::string &shell)
{
    std::string result = "";
    result += sequence_begin.at(shell);
    result += csi;
    result += code;
    result += 'm';
    result += sequence_end.at(shell);
    return result;
}

std::string to_color_code(const std::string &color, const int offset, const std::string &shell)
{
    if (color.empty())
    {
        return "";
    }
    else if (foreground_color_map.find(color) != foreground_color_map.end())
    {
        const int code = foreground_color_map.at(color);
        return create_sequence(std::to_string(code + offset), shell);
    }
    else
    {
        const std::string rgb_prefix = offset == 0 ? foreground_rgb_prefix : background_rgb_prefix;
        return create_sequence(rgb_prefix + color, shell);
    }
}

std::string set_text_mode(const int code, const std::string &shell)
{
    return create_sequence(std::to_string(code), shell);
}

std::string set_foreground(const std::string &color, const std::string &shell)
{
    return to_color_code(color, 0, shell);
}

std::string set_background(const std::string &color, const std::string &shell)
{
    return to_color_code(color, BACKGROUND_COLOR_OFFSET, shell);
}

std::string reset_all(const std::string &shell)
{
    return create_sequence(std::to_string(RESET), shell);
}

std::string reset_text_mode(const int code, const std::string &shell)
{
    int special_offset = 0;
    if (code == BOLD)
    {
        special_offset = 1;
    }
    return create_sequence(std::to_string(code + TEXT_TYPE_RESET_OFFSET + special_offset), shell);
}

std::string reset_foreground(const std::string &shell)
{
    return create_sequence(std::to_string(DEFAULT), shell);
}

std::string reset_background(const std::string &shell)
{
    return create_sequence(std::to_string(DEFAULT + BACKGROUND_COLOR_OFFSET), shell);
}
