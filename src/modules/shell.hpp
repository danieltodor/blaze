#pragma once

#include <string>

#include "context.hpp"

// Currently active shell
std::string shell(const Context &context)
{
    return context.args.shell;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("shell")
{
    Context context;
    context.args.shell = "abc";
    CHECK(shell(context) == "abc");
}

#endif
