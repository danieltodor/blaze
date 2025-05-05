#pragma once

#include <string>

#include "context.hpp"

// Call the module function by its name
std::string call_module(const std::string &name, const Context &context);
