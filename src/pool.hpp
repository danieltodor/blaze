#include "external/thread-pool/BS_thread_pool.hpp"

#define THREAD_LIMIT 4

static BS::thread_pool thread_pool(std::min((int)std::thread::hardware_concurrency(), THREAD_LIMIT));
