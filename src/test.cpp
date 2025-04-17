#define DOCTEST_CONFIG_IMPLEMENT

#include "test.hpp"

#ifdef TEST
#include <cstdlib>

void run_tests(int argc, const char* const* argv)
{
    doctest::Context context;
    context.setOption("no-breaks", true);
    context.applyCommandLine(argc, argv);
    int result = context.run();
    if (context.shouldExit())
    {
        std::exit(result);
    }
}

#endif
