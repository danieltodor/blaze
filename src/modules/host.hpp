#pragma once

#include <string>

#include "context.hpp"
#include "util.hpp"

// Hostname
std::string host(const Context &context)
{
    (void)context;
    std::string result = "";
    result += execute_command("cat /etc/hostname");
    strip(result);
    return result;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("host")
{
    Context context;
    CHECK(host(context).length() > 0);
}

#endif
