#pragma once

#include "thread-pool/BS_thread_pool.hpp"

#define THREAD_LIMIT 4U

inline BS::thread_pool thread_pool(std::min(std::thread::hardware_concurrency(), THREAD_LIMIT));

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("thread count")
{
    const std::size_t thread_count = thread_pool.get_thread_count();
    CHECK(thread_count > 0);
    CHECK(thread_count <= THREAD_LIMIT);
}

#endif
