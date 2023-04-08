#include <cstdlib>

#define DOCTEST_CONFIG_IMPLEMENT
#include "src/test.hpp"

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
