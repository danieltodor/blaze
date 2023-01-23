#ifndef BLAZE_UTIL_HPP_INCLUDED
#define BLAZE_UTIL_HPP_INCLUDED

#include <string>
#include <vector>

void regex_replace(std::string &string, const std::vector<std::string> patterns, const std::string &replacement);
bool regex_search(std::string &string, const std::vector<std::string> patterns);
void strip(std::string &string);
std::string execute_command(std::string command);
bool git_repository_detected();

#endif
