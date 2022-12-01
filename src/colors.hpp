#include <string>

// Colors
#define RESET 0
#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37

// Text types
#define NORMAL 0
#define BOLD 1
#define UNDERLINED 4

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

std::string fg(int color_code, int text_type=NORMAL)
{
    return color(text_type, color_code);
}

std::string reset()
{
    return color(NORMAL, RESET);
}
