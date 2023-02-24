#include <atomic>
#include <iostream>
#include <Common/ThreadPool.h>

#include <gtest/gtest.h>


TEST(ThreadPool, Loop)
{
    std::atomic<int> res{0};

    for (unsigned long i = 0; i < 1000; ++i)
    {
        unsigned long threads = 16;
        ThreadPool pool(threads);
        for (unsigned long j = 0; j < threads; ++j)
            pool.scheduleOrThrowOnError([&] { ++res; });
        pool.wait();
    }

    EXPECT_EQ(res, 16000);
}
