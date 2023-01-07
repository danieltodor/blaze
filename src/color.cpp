#include <unordered_map>

#include "color.hpp"

const std::string csi = "\001\033["; // Control Sequence Introducer
const std::string rgb_background_prefix = "48;2;";
const std::string rgb_foreground_prefix = "38;2;";

typedef std::unordered_map<std::string, std::string> stringmap;
const stringmap foreground_color_map = {
    {"black", std::to_string(BLACK)},
    {"red", std::to_string(RED)},
    {"green", std::to_string(GREEN)},
    {"yellow", std::to_string(YELLOW)},
    {"blue", std::to_string(BLUE)},
    {"magenta", std::to_string(MAGENTA)},
    {"cyan", std::to_string(CYAN)},
    {"white", std::to_string(WHITE)}
};
const stringmap background_color_map = {
    {"black", std::to_string(BLACK + 10)},
    {"red", std::to_string(RED + 10)},
    {"green", std::to_string(GREEN + 10)},
    {"yellow", std::to_string(YELLOW + 10)},
    {"blue", std::to_string(BLUE + 10)},
    {"magenta", std::to_string(MAGENTA + 10)},
    {"cyan", std::to_string(CYAN + 10)},
    {"white", std::to_string(WHITE + 10)}
};

std::string graphics(std::string code)
{
    std::string result = csi;
    result += code;
    result += "m\002";
    return result;
}

std::string to_color_code(std::string color, stringmap color_map, std::string rgb_prefix)
{
    if (color.empty())
    {
        return "";
    }
    else if (color_map.find(color) != color_map.end())
    {
        return graphics(color_map.at(color));
    }
    else
    {
        return graphics(rgb_prefix + color);
    }
}

std::string text_mode(int code)
{
    if (code == 0)
    {
        return "";
    }
    return graphics(std::to_string(code));
}

std::string background(std::string color)
{
    return to_color_code(color, background_color_map, rgb_background_prefix);
}

std::string foreground(std::string color)
{
    return to_color_code(color, foreground_color_map, rgb_foreground_prefix);
}

std::string reset()
{
    return graphics(std::to_string(RESET));
}
