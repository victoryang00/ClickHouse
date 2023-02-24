#include <iomanip>
#include <pcg_random.hpp>
#include <base/bit_cast.h>

//#if defined(NDEBUG)
//#undef NDEBUG
#include <Common/RadixSort.h>
//#endif

#include <Common/Stopwatch.h>
#include <Common/randomSeed.h>
#include <IO/ReadHelpers.h>
#include <Core/Defines.h>
#include <pdqsort.h>

/// Example:
/// for i in {6,8} {11..26}; do echo $i; for j in {1..10}; do ./radix_sort $i 65536 1000; done; echo; done


using Key = UInt64;

static void NO_INLINE sort1(Key * data, unsigned long size)
{
    std::sort(data, data + size);
}

static void NO_INLINE sort2(Key * data, unsigned long size)
{
    radixSortLSD(data, size);
}

static void NO_INLINE sort3(Key * data, unsigned long size)
{
    std::sort(data, data + size, [](Key a, Key b)
    {
        return RadixSortFloatTransform<uint64_t>::forward(bit_cast<uint64_t>(a))
            < RadixSortFloatTransform<uint64_t>::forward(bit_cast<uint64_t>(b));
    });
}

static void NO_INLINE sort4(Key * data, unsigned long size)
{
    radixSortMSD(data, size, size);
}

static void NO_INLINE sort5(Key * data, unsigned long size)
{
    pdqsort(data, data + size);
}


static void NO_INLINE sort6(Key * data, unsigned long size, unsigned long limit)
{
    std::partial_sort(data, data + limit, data + size);
}

static void NO_INLINE sort7(Key * data, unsigned long size, unsigned long limit)
{
    std::partial_sort(data, data + limit, data + size, [](Key a, Key b)
    {
        return RadixSortFloatTransform<uint64_t>::forward(bit_cast<uint64_t>(a))
            < RadixSortFloatTransform<uint64_t>::forward(bit_cast<uint64_t>(b));
    });
}

static void NO_INLINE sort8(Key * data, unsigned long size, unsigned long limit)
{
    radixSortMSD(data, size, limit);
}


template <unsigned long N>
struct RadixSortTraitsWithCustomBits : RadixSortNumTraits<Key>
{
    static constexpr unsigned long PART_SIZE_BITS = N;
};

static void NO_INLINE sort11(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<1>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort12(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<2>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort13(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<3>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort14(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<4>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort15(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<5>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort16(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<6>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort17(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<7>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort18(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<8>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort19(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<9>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort20(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<10>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort21(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<11>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort22(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<12>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort23(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<13>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort24(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<14>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort25(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<15>>::executeMSD(data, size, limit);
}

static void NO_INLINE sort26(Key * data, unsigned long size, unsigned long limit)
{
    RadixSort<RadixSortTraitsWithCustomBits<16>>::executeMSD(data, size, limit);
}


int main(int argc, char ** argv)
{
    pcg64 rng(randomSeed());

    if (argc < 3 || argc > 4)
    {
        std::cerr << "Usage: program method n [limit]\n";
        return 1;
    }

    unsigned long method = DB::parse<unsigned long>(argv[1]);
    unsigned long n = DB::parse<unsigned long>(argv[2]);
    unsigned long limit = n;

    if (argc == 4)
        limit = DB::parse<unsigned long>(argv[3]);

    std::cerr << std::fixed << std::setprecision(3);

    std::vector<Key> data(n);

    {
        Stopwatch watch;

        for (auto & elem : data)
            elem = rng();

        watch.stop();
    /*    double elapsed = watch.elapsedSeconds();
        std::cerr
            << "Filled in " << elapsed
            << " (" << n / elapsed << " elem/sec., "
            << n * sizeof(Key) / elapsed / 1048576 << " MB/sec.)"
            << std::endl;*/
    }

    if (n <= 100)
    {
        std::cerr << std::endl;
        for (const auto & elem : data)
            std::cerr << elem << ' ';
        std::cerr << std::endl;
    }

    {
        Stopwatch watch;

        if (method == 1) sort1(data.data(), n);
        if (method == 2) sort2(data.data(), n);
        if (method == 3) sort3(data.data(), n);
        if (method == 4) sort4(data.data(), n);
        if (method == 5) sort5(data.data(), n);
        if (method == 6) sort6(data.data(), n, limit);
        if (method == 7) sort7(data.data(), n, limit);
        if (method == 8) sort8(data.data(), n, limit);

        if (method == 11) sort11(data.data(), n, limit);
        if (method == 12) sort12(data.data(), n, limit);
        if (method == 13) sort13(data.data(), n, limit);
        if (method == 14) sort14(data.data(), n, limit);
        if (method == 15) sort15(data.data(), n, limit);
        if (method == 16) sort16(data.data(), n, limit);
        if (method == 17) sort17(data.data(), n, limit);
        if (method == 18) sort18(data.data(), n, limit);
        if (method == 19) sort19(data.data(), n, limit);
        if (method == 20) sort20(data.data(), n, limit);
        if (method == 21) sort21(data.data(), n, limit);
        if (method == 22) sort22(data.data(), n, limit);
        if (method == 23) sort23(data.data(), n, limit);
        if (method == 24) sort24(data.data(), n, limit);
        if (method == 25) sort25(data.data(), n, limit);
        if (method == 26) sort26(data.data(), n, limit);

        watch.stop();
        double elapsed = watch.elapsedSeconds();
        std::cerr
            << "Sorted in " << elapsed
            << " (" << n / elapsed << " elem/sec., "
            << n * sizeof(Key) / elapsed / 1048576 << " MB/sec.)"
            << std::endl;
    }

    bool ok = true;

    {
        Stopwatch watch;

        unsigned long i = 1;
        while (i < limit)
        {
            if (!(data[i - 1] <= data[i]))
            {
                ok = false;
                break;
            }
            ++i;
        }

        watch.stop();
        double elapsed = watch.elapsedSeconds();
        if (!ok)
            std::cerr
                << "Checked in " << elapsed
                << " (" << limit / elapsed << " elem/sec., "
                << limit * sizeof(Key) / elapsed / 1048576 << " MB/sec.)"
                << std::endl
                << "Result: " << (ok ? "Ok." : "Fail!") << std::endl;
    }

    if (!ok && limit <= 100000)
    {
        std::cerr << std::endl;

        std::cerr << data[0] << ' ';
        for (unsigned long i = 1; i < limit; ++i)
        {
            if (!(data[i - 1] <= data[i]))
                std::cerr << "*** ";
            std::cerr << data[i] << ' ';
        }

        std::cerr << std::endl;
    }

    return 0;
}
