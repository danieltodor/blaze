#include "external/thread-pool/BS_thread_pool.hpp"

#define THREAD_LIMIT 4U

static BS::thread_pool thread_pool(std::min(std::thread::hardware_concurrency(), THREAD_LIMIT));

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("thread count")
{
    CHECK(thread_pool.get_thread_count() <= THREAD_LIMIT);
}

#endif
