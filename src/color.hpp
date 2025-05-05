#pragma once

#include "types.hpp"
#include "context.hpp"

// Text modes
#define BOLD 1
#define DIM 2
#define ITALIC 3
#define UNDERLINE 4

// Set text mode like bold, italic, etc
std::string set_text_mode(const int code, const Context &context);

// Set foreground color
std::string set_foreground(const std::string &color, const Context &context);

// Set background color
std::string set_background(const std::string &color, const Context &context);

// Reset all attributes
std::string reset_all(const Context &context);

// Reset selected text mode
std::string reset_text_mode(const int code, const Context &context);

// Reset foreground color
std::string reset_foreground(const Context &context);

// Reset background color
std::string reset_background(const Context &context);

// Move cursor up by x lines
std::string move_cursor_up(const int lines, const Context &context);

// Erase from cursor until end of screen
std::string erase_until_end_of_screen(const Context &context);

// Get sequence characters
StringVector get_sequence_characters(const Context &context);
