#ifndef BLAZE_UTIL_HPP_INCLUDED
#define BLAZE_UTIL_HPP_INCLUDED

#include <string>
#include <vector>
#include <time.h>

// Number of columns currently in the terminal emulator
unsigned short get_columns();

// Number of displayed characters in the strings
std::size_t get_length(const std::vector<std::string> &strings);

// Split the string at delimiters
std::vector<std::string> split(const std::string &string, const std::string &delimiter);

// Join the strings at delimiters
std::string join(const std::vector<std::string> &strings, const std::string &delimiter);

// Replace parts in-place
void regex_replace(std::string &string, const std::vector<std::string> &patterns, const std::string &replacement);

// Find all occurences
std::vector<std::string> regex_find_all(const std::string &string, const std::vector<std::string> &patterns);

// Strip whitespace from beginning/end of string
void strip(std::string &string);

// Execute command in shell
std::string execute_command(const std::string &command, int *exit_status=NULL);

// Check if git commands can be used in the current directory
bool check_git_repository();

// Read ENV variables. Always returns string to allow concatenation
std::string get_env(const std::string &name);

// System time
std::tm get_current_time();

// Formatted time representation
std::string format_time(const std::tm &time_structure, const std::string &format);

// Check if every character in the string is a digit
bool is_number(const std::string &string);

#endif
