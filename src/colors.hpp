#ifndef BLAZE_COLORS_HPP_INCLUDED
#define BLAZE_COLORS_HPP_INCLUDED

#include <string>

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

std::string text_mode(int code);
std::string background(std::string color);
std::string foreground(std::string color);
std::string reset();

#endif
