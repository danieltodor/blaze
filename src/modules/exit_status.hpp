#ifndef BLAZE_MODULES_EXIT_STATUS_HPP_INCLUDED
#define BLAZE_MODULES_EXIT_STATUS_HPP_INCLUDED

#include <string>

#include "src/context.hpp"

// Exit status of the last command
std::string exit_status(const Context &context)
{
    std::string result = "";
    if (context.args.exit_status != 0 || !context.config.exit_status.non_zero_only)
    {
        result += std::to_string(context.args.exit_status);
    }
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("exit_status")
{
    Context context;
    context.config.exit_status.non_zero_only = true;
    context.args.exit_status = 0;
    SUBCASE("empty if exit_status is zero and non_zero_only is true")
    {
        const std::string result = exit_status(context);
        CHECK(result == "");
    }
    SUBCASE("correct exit_status 0")
    {
        context.config.exit_status.non_zero_only = false;
        const std::string result = exit_status(context);
        CHECK(result == "0");
    }
    SUBCASE("correct positive exit_status")
    {
        context.args.exit_status = 255;
        const std::string result = exit_status(context);
        CHECK(result == "255");
    }
    SUBCASE("correct negative exit_status")
    {
        context.args.exit_status = -255;
        const std::string result = exit_status(context);
        CHECK(result == "-255");
    }
}

#endif

#endif
