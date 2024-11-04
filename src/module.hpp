#ifndef BLAZE_MODULE_HPP_INCLUDED
#define BLAZE_MODULE_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Call the module function by its name
std::string call_module(const std::string &name, const Context &context);

#endif
