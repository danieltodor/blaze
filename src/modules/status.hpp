#ifndef BLAZE_MODULES_STATUS_HPP_INCLUDED
#define BLAZE_MODULES_STATUS_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Exit status of the last command
std::string status(const Context &context)
{
    std::string result = "";
    if (context.args.status != 0 || context.config.status.show_zero)
    {
        result += std::to_string(context.args.status);
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("status")
{
    Context context;
    context.config.status.show_zero = false;
    context.args.status = 0;
    SUBCASE("empty if status is zero and show_zero is false")
    {
        CHECK(status(context) == "");
    }
    SUBCASE("correct status 0")
    {
        context.config.status.show_zero = true;
        CHECK(status(context) == "0");
    }
    SUBCASE("correct positive status")
    {
        context.args.status = 255;
        CHECK(status(context) == "255");
    }
    SUBCASE("correct negative status")
    {
        context.args.status = -255;
        CHECK(status(context) == "-255");
    }
}

#endif

#endif
