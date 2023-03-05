#ifndef BLAZE_UTIL_HPP_INCLUDED
#define BLAZE_UTIL_HPP_INCLUDED

#include <string>
#include <vector>
#include <time.h>

unsigned short get_column();
std::size_t get_length(const std::vector<std::string> &strings);
std::vector<std::string> split(const std::string &string, const std::string &delimiter);
std::string join(const std::vector<std::string> &strings, const std::string &delimiter);
void regex_replace(std::string &string, const std::vector<std::string> &patterns, const std::string &replacement);
std::vector<std::string> regex_find_all(const std::string &string, const std::vector<std::string> &patterns);
void strip(std::string &string);
std::string execute_command(const std::string &command);
bool check_git_repository();
std::string get_env(const std::string &name);
std::tm get_current_time();
std::string format_time(const std::tm &time_structure, const std::string &format);
bool is_number(const std::string &string);

#endif
