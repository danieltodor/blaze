#ifndef BLAZE_MODULES_SHELL_HPP_INCLUDED
#define BLAZE_MODULES_SHELL_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Currently active shell
std::string shell(const Context &context)
{
    return context.args.shell;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "src/test.hpp"

TEST_CASE("shell")
{
    Context context;
    context.args.shell = "abc";
    CHECK(shell(context) == "abc");
}

#endif

#endif
