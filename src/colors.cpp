#include "colors.hpp"

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

std::string color(int text_type, int color_code)
{
    std::string result = "\033[";
    result += std::to_string(text_type);
    result += std::to_string(color_code);
    result += 'm';
    return result;
}

std::string bg(int color_code)
{
    int bg_color_code = color_code + 10;
    return color(NORMAL, bg_color_code);
}

std::string fg(int color_code, int text_type)
{
    return color(text_type, color_code);
}

std::string reset()
{
    return color(NORMAL, RESET);
}
