#ifndef BLAZE_UTIL_HPP_INCLUDED
#define BLAZE_UTIL_HPP_INCLUDED

#include <string>
#include <vector>

unsigned short get_column();
std::size_t get_length(std::vector<std::string> strings);
std::vector<std::string> split(const std::string &string, const std::string &delimiter);
std::string join(const std::vector<std::string> &strings, const std::string &delimiter);
void regex_replace(std::string &string, const std::vector<std::string> patterns, const std::string &replacement);
bool regex_search(std::string &string, const std::vector<std::string> patterns);
void strip(std::string &string);
std::string execute_command(std::string command);
bool check_git_repository();

#endif
