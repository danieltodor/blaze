#ifndef BLAZE_PRINT_HPP_INCLUDED
#define BLAZE_PRINT_HPP_INCLUDED

#include "src/context.hpp"

// Print the modules and the prompt
void print_prompt(Context &context);

// Print the right prompt
void print_right_prompt(Context &context);

// Print the transient prompt
void print_transient_prompt(Context &context);

#endif
