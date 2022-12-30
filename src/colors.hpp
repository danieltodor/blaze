#ifndef BLAZE_COLORS_HPP_INCLUDED
#define BLAZE_COLORS_HPP_INCLUDED

#include <string>
#include <unordered_map>

#define RESET 0

// Colors
#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37

// Text types
#define BOLD 1
#define DIM 2
#define ITALIC 3
#define UNDERLINE 4

std::unordered_map<std::string, int> color_map();
std::string graphics(int code);
std::string text_mode(int code);
std::string background(int color_code);
std::string foreground(int color_code);
std::string reset();

#endif
