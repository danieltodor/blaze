#ifndef BLAZE_UTILS_HPP_INCLUDED
#define BLAZE_UTILS_HPP_INCLUDED

#include <string>
#include <vector>

void regex_replace(std::string &string, const std::vector<std::string> patterns, const std::string &replacement);
bool regex_search(std::string &string, const std::vector<std::string> patterns);
void strip(std::string &string);

#endif
