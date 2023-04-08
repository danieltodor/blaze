#ifndef BLAZE_TEST_HPP_INCLUDED
#define BLAZE_TEST_HPP_INCLUDED

#define DOCTEST_CONFIG_DISABLE // Remove everything test related from binary
#include "external/doctest/doctest.h"

// Run test cases
void run_tests(int argc, const char* const* argv);

#endif
