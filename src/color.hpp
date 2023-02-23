#ifndef BLAZE_COLOR_HPP_INCLUDED
#define BLAZE_COLOR_HPP_INCLUDED

#include <string>

#include "context.hpp"

#define RESET 0
#define BACKGROUND_COLOR_OFFSET 10
#define TEXT_TYPE_RESET_OFFSET 20
#define DEFAULT_COLOR 39

// Normal colors
#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37

// Bright colors
#define BRIGHT_BLACK 90
#define BRIGHT_RED 91
#define BRIGHT_GREEN 92
#define BRIGHT_YELLOW 93
#define BRIGHT_BLUE 94
#define BRIGHT_MAGENTA 95
#define BRIGHT_CYAN 96
#define BRIGHT_WHITE 97

// Text types
#define BOLD 1
#define DIM 2
#define ITALIC 3
#define UNDERLINE 4

std::string set_text_mode(const int code, const Context &context);
std::string set_foreground(const std::string &color, const Context &context);
std::string set_background(const std::string &color, const Context &context);
std::string reset_all(const Context &context);
std::string reset_text_mode(const int code, const Context &context);
std::string reset_foreground(const Context &context);
std::string reset_background(const Context &context);

#endif
