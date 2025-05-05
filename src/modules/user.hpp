#pragma once

#include <string>

#include "context.hpp"
#include "util.hpp"

// Current user
std::string user(const Context &context)
{
    (void)context;
    return get_env("USER");
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("user")
{
    Context context;
    CHECK(user(context).length() > 0);
}

#endif
