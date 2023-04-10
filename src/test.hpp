#ifndef BLAZE_TEST_HPP_INCLUDED
#define BLAZE_TEST_HPP_INCLUDED

// #define TEST // Enable test runner
#ifdef TEST

#include "external/doctest/doctest.h"

// Run test cases
void run_tests(int argc, const char* const* argv);

#endif

#endif
