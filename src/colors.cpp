#include "colors.hpp"

const std::string csi = "\033["; // Control Sequence Introducer

std::unordered_map<std::string, std::string> color_map()
{
    std::unordered_map<std::string, std::string> map;
    map["black"] = std::to_string(BLACK);
    map["red"] = std::to_string(RED);
    map["green"] = std::to_string(GREEN);
    map["yellow"] = std::to_string(YELLOW);
    map["blue"] = std::to_string(BLUE);
    map["magenta"] = std::to_string(MAGENTA);
    map["cyan"] = std::to_string(CYAN);
    map["white"] = std::to_string(WHITE);
    return map;
}

std::string graphics(std::string code)
{
    std::string result = csi;
    result += code;
    result += 'm';
    return result;
}

std::string text_mode(int code)
{
    if (code == 0) {return "";}
    return graphics(std::to_string(code));
}

std::string background(std::string color_code)
{
    if (color_code.empty()) {return "";}
    std::string bg_color_code = std::to_string(std::stoi(color_code) + 10);
    return graphics(bg_color_code);
}

std::string foreground(std::string color_code)
{
    if (color_code.empty()) {return "";}
    return graphics(color_code);
}

std::string reset()
{
    return graphics(std::to_string(RESET));
}
