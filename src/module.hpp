#pragma once

#include <string>

#include "context.hpp"

// Virtual modules
#define FIXED_MODULE "fixed"
#define SEPARATOR_MODULE "separator"

// Call the module function by its name
std::string call_module(const std::string &name, const Context &context);
