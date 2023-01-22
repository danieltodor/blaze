#ifndef BLAZE_UTILS_HPP_INCLUDED
#define BLAZE_UTILS_HPP_INCLUDED

#include <string>
#include <regex>

void regex_replace(std::string &string, const std::string patterns[], const std::string &replacement);
void strip(std::string &string);

#endif
