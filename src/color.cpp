#include <unordered_map>

#include "color.hpp"

const std::string csi = "\033["; // Control Sequence Introducer
const std::string rgb_foreground_prefix = "38;2;";
const std::string rgb_background_prefix = "48;2;";
const std::unordered_map<std::string, std::string> shell_begin = {
    {"bash", "\001"} // \[
};
const std::unordered_map<std::string, std::string> shell_end = {
    {"bash", "\002"} // \]
};
const std::unordered_map<std::string, std::string> foreground_color_map = {
    {"black", std::to_string(BLACK)},
    {"red", std::to_string(RED)},
    {"green", std::to_string(GREEN)},
    {"yellow", std::to_string(YELLOW)},
    {"blue", std::to_string(BLUE)},
    {"magenta", std::to_string(MAGENTA)},
    {"cyan", std::to_string(CYAN)},
    {"white", std::to_string(WHITE)}
};

std::string graphics(const std::string &code, const std::string &shell)
{
    std::string result = "";
    result += shell_begin.at(shell);
    result += csi;
    result += code;
    result += 'm';
    result += shell_end.at(shell);
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
        const int color_code = std::stoi(foreground_color_map.at(color));
        return graphics(std::to_string(color_code + offset), shell);
    }
    else
    {
        const std::string rgb_prefix = offset == 0 ? rgb_foreground_prefix : rgb_background_prefix;
        return graphics(rgb_prefix + color, shell);
    }
}

std::string set_text_mode(const int code, const std::string &shell)
{
    return graphics(std::to_string(code), shell);
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
    return graphics(std::to_string(RESET), shell);
}

std::string reset_text_mode(const int code, const std::string &shell)
{
    int special_offset = 0;
    if (code == BOLD)
    {
        special_offset = 1;
    }
    return graphics(std::to_string(code + TEXT_TYPE_RESET_OFFSET + special_offset), shell);
}

std::string reset_foreground(const std::string &shell)
{
    return graphics(std::to_string(DEFAULT), shell);
}

std::string reset_background(const std::string &shell)
{
    return graphics(std::to_string(DEFAULT + BACKGROUND_COLOR_OFFSET), shell);
}
