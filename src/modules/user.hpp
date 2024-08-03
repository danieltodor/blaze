#ifndef BLAZE_MODULES_USER_HPP_INCLUDED
#define BLAZE_MODULES_USER_HPP_INCLUDED

#include <string>

#include "src/context.hpp"
#include "src/util.hpp"

// Current user
std::string user(const Context &context)
{
    (void)context;
    std::string result = "";
    result += get_env("USER");
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("user")
{
    Context context;
    const std::string result = user(context);
    CHECK(result.length() > 0);
}

#endif

#endif
