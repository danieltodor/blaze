#include "colors.hpp"

const std::string csi = "\033["; // Control Sequence Introducer

std::unordered_map<std::string, int> color_map()
{
    std::unordered_map<std::string, int> map;
    map["none"] = RESET;
    map["black"] = BLACK;
    map["red"] = RED;
    map["green"] = GREEN;
    map["yellow"] = YELLOW;
    map["blue"] = BLUE;
    map["magenta"] = MAGENTA;
    map["cyan"] = CYAN;
    map["white"] = WHITE;
    return map;
}

std::string graphics(int code)
{
    std::string result = csi;
    result += std::to_string(code);
    result += 'm';
    return result;
}

std::string text_mode(int code)
{
    return graphics(code);
}

std::string background(int color_code)
{
    int bg_color_code = color_code + 10;
    return graphics(bg_color_code);
}

std::string foreground(int color_code)
{
    return graphics(color_code);
}

std::string reset()
{
    return graphics(RESET);
}
