#include <iostream>
#include <cstring>
#include <thread>
#include <pcg_random.hpp>
#include <Common/ArrayCache.h>
#include <IO/ReadHelpers.h>


template <typename Cache>
void printStats(const Cache & cache)
{
    typename Cache::Statistics statistics = cache.getStatistics();
    std::cerr
        << "total_chunks_size: " << statistics.total_chunks_size << "\n"
        << "total_allocated_size: " << statistics.total_allocated_size << "\n"
        << "total_size_currently_initialized: " << statistics.total_size_currently_initialized << "\n"
        << "total_size_in_use: " << statistics.total_size_in_use << "\n"
        << "num_chunks: " << statistics.num_chunks << "\n"
        << "num_regions: " << statistics.num_regions << "\n"
        << "num_free_regions: " << statistics.num_free_regions << "\n"
        << "num_regions_in_use: " << statistics.num_regions_in_use << "\n"
        << "num_keyed_regions: " << statistics.num_keyed_regions << "\n"
        << "hits: " << statistics.hits << "\n"
        << "concurrent_hits: " << statistics.concurrent_hits << "\n"
        << "misses: " << statistics.misses << "\n"
        << "allocations: " << statistics.allocations << "\n"
        << "allocated_bytes: " << statistics.allocated_bytes << "\n"
        << "evictions: " << statistics.evictions << "\n"
        << "evicted_bytes: " << statistics.evicted_bytes << "\n"
        << "secondary_evictions: " << statistics.secondary_evictions << "\n"
        << "\n";
}

/** Example:
  * time ./array_cache 68000000 1 10000000 2000000 200
  */


int main(int argc, char ** argv)
{
    if (argc < 6)
    {
        std::cerr << "Usage: program cache_size num_threads num_iterations region_max_size max_key\n";
        return 1;
    }

    unsigned long cache_size = DB::parse<unsigned long>(argv[1]);
    unsigned long num_threads = DB::parse<unsigned long>(argv[2]);
    unsigned long num_iterations = DB::parse<unsigned long>(argv[3]);
    unsigned long region_max_size = DB::parse<unsigned long>(argv[4]);
    unsigned long max_key = DB::parse<unsigned long>(argv[5]);

    using Cache = ArrayCache<int, int>;
    Cache cache(cache_size);

    std::vector<std::thread> threads;
    for (unsigned long i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([&]
        {
            pcg64 generator(randomSeed());

            for (unsigned long j = 0; j < num_iterations; ++j)
            {
                unsigned long size = std::uniform_int_distribution<unsigned long>(1, region_max_size)(generator);
                int key = std::uniform_int_distribution<int>(1, max_key)(generator);

                cache.getOrSet(
                    key,
                    [=]{ return size; },
                    [=](void * /*ptr*/, int & payload)
                    {
                        payload = j;
                //        memset(ptr, j, size);
                    },
                    nullptr);

            //    printStats(cache);
            }
        });
    }

    std::atomic_bool stop{};

    std::thread stats_thread([&]
    {
        while (!stop)
        {
            usleep(100000);
            printStats(cache);
        }
    });

    for (auto & thread : threads)
        thread.join();

    stop = true;
    stats_thread.join();

    return 0;

/*
    using Cache = ArrayCache<int, int>;
    Cache cache(64 * 1024 * 1024);

    cache.getOrSet(
        1,
        [=]{ return 32 * 1024 * 1024; },
        [=](void * ptr, int & payload)
        {
            payload = 1;
        },
        nullptr);

    printStats(cache);

    cache.getOrSet(
        2,
        [=]{ return 32 * 1024 * 1024; },
        [=](void * ptr, int & payload)
        {
            payload = 2;
        },
        nullptr);

    printStats(cache);

    cache.getOrSet(
        3,
        [=]{ return 32 * 1024 * 1024; },
        [=](void * ptr, int & payload)
        {
            payload = 3;
        },
        nullptr);

    printStats(cache);*/
}
