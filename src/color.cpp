#include <unordered_map>

#include "color.hpp"

const std::string csi = "\001\033["; // Control Sequence Introducer
const std::string rgb_foreground_prefix = "38;2;";
const std::string rgb_background_prefix = "48;2;";

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

std::string graphics(const std::string &code)
{
    std::string result = csi;
    result += code;
    result += "m\002";
    return result;
}

std::string to_color_code(const std::string &color, const int offset = 0)
{
    if (color.empty())
    {
        return "";
    }
    else if (foreground_color_map.find(color) != foreground_color_map.end())
    {
        const int color_code = std::stoi(foreground_color_map.at(color));
        return graphics(std::to_string(color_code + offset));
    }
    else
    {
        const std::string rgb_prefix = offset == 0 ? rgb_foreground_prefix : rgb_background_prefix;
        return graphics(rgb_prefix + color);
    }
}

std::string set_text_mode(const int code)
{
    return graphics(std::to_string(code));
}

std::string set_foreground(const std::string &color)
{
    return to_color_code(color);
}

std::string set_background(const std::string &color)
{
    return to_color_code(color, BACKGROUND_COLOR_OFFSET);
}

std::string reset_all()
{
    return graphics(std::to_string(RESET));
}

std::string reset_text_mode(const int code)
{
    int special_offset = 0;
    if (code == BOLD)
    {
        special_offset = 1;
    }
    return graphics(std::to_string(code + TEXT_TYPE_RESET_OFFSET + special_offset));
}

std::string reset_foreground()
{
    return graphics(std::to_string(DEFAULT));
}

std::string reset_background()
{
    return graphics(std::to_string(DEFAULT + BACKGROUND_COLOR_OFFSET));
}
