#ifndef BLAZE_MODULES_DUMMY_HPP_INCLUDED
#define BLAZE_MODULES_DUMMY_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Dummy module that can be used for multiple purposes
std::string dummy(const Context &context)
{
    (void)context;
    std::string result = "";
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("dummy")
{
    Context context;
    CHECK(dummy(context) == "");
}

#endif

#endif
