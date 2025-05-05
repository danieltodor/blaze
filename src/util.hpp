#pragma once

#include <time.h>
#include <sys/ioctl.h>

#include "types.hpp"

// Terminal window size
winsize get_winsize();

// Number of displayed characters in the strings
std::size_t get_length(const StringVector &strings);

// Find the start index of the nth occurrence of a substring in a string
std::size_t find_nth_occurrence(const std::string &string, const std::string &substring, const std::size_t n, const bool reverse=false);

// Split the string at delimiters
StringVector split(const std::string &string, const std::string &delimiter);

// Join the strings at delimiters
std::string join(const StringVector &strings, const std::string &delimiter);

// Multiply string n times
std::string multiply_string(const int number, const std::string &string);

// Replace parts in-place
void regex_replace(std::string &string, const StringVector &patterns, const std::string &replacement);

// Find all occurences
StringVector regex_find_all(const std::string &string, const StringVector &patterns);

// Strip whitespace from beginning/end of string
void strip(std::string &string);

// Execute command in shell
std::string execute_command(const std::string &command, int *status=nullptr);

// Check the presence and state of the current repository
void check_git_repository(bool &detected, bool &detached);

// Read ENV variables. Always returns string to allow concatenation
std::string get_env(const std::string &name);

// System time
std::tm get_current_time();

// Formatted time representation
std::string format_time(const std::tm &time_structure, const std::string &format);

// Check if every character in the string is a digit
bool is_number(const std::string &string);
