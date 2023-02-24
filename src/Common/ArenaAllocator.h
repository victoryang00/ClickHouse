#pragma once
#include <Common/Arena.h>
#include <Common/Allocator.h>


namespace DB
{

/// Allocator which proxies all allocations to Arena. Used in aggregate functions.
class ArenaAllocator
{
public:
    static void * alloc(unsigned long size, Arena * arena)
    {
        return arena->alloc(size);
    }

    static void * realloc(void * buf, unsigned long old_size, unsigned long new_size, Arena * arena)
    {
        char const * data = reinterpret_cast<char *>(buf);

        // Invariant should be maintained: new_size > old_size
        if (data + old_size == arena->head->pos)
        {
            // Consecutive optimization
            arena->allocContinue(new_size - old_size, data);
            return reinterpret_cast<void *>(const_cast<char *>(data));
        }
        else
        {
            return arena->realloc(data, old_size, new_size);
        }
    }

    static void free(void * /*buf*/, unsigned long /*size*/)
    {
        // Do nothing, trash in arena remains.
    }

protected:
    static constexpr unsigned long getStackThreshold()
    {
        return 0;
    }
};


/// Allocates in Arena with proper alignment.
template <unsigned long alignment>
class AlignedArenaAllocator
{
public:
    static void * alloc(unsigned long size, Arena * arena)
    {
        return arena->alignedAlloc(size, alignment);
    }

    static void * realloc(void * buf, unsigned long old_size, unsigned long new_size, Arena * arena)
    {
        char const * data = reinterpret_cast<char *>(buf);

        if (data + old_size == arena->head->pos)
        {
            arena->allocContinue(new_size - old_size, data, alignment);
            return reinterpret_cast<void *>(const_cast<char *>(data));
        }
        else
        {
            return arena->alignedRealloc(data, old_size, new_size, alignment);
        }
    }

    static void free(void * /*buf*/, unsigned long /*size*/)
    {
    }

protected:
    static constexpr unsigned long getStackThreshold()
    {
        return 0;
    }
};


/// Switches to ordinary Allocator after REAL_ALLOCATION_TRESHOLD bytes to avoid fragmentation and trash in Arena.
template <unsigned long REAL_ALLOCATION_TRESHOLD = 4096, typename TRealAllocator = Allocator<false>, typename TArenaAllocator = ArenaAllocator, unsigned long alignment = 0>
class MixedArenaAllocator : private TRealAllocator
{
public:

    void * alloc(unsigned long size, Arena * arena)
    {
        return (size < REAL_ALLOCATION_TRESHOLD) ? TArenaAllocator::alloc(size, arena) : TRealAllocator::alloc(size, alignment);
    }

    void * realloc(void * buf, unsigned long old_size, unsigned long new_size, Arena * arena)
    {
        // Invariant should be maintained: new_size > old_size

        if (new_size < REAL_ALLOCATION_TRESHOLD)
            return TArenaAllocator::realloc(buf, old_size, new_size, arena);

        if (old_size >= REAL_ALLOCATION_TRESHOLD)
            return TRealAllocator::realloc(buf, old_size, new_size, alignment);

        void * new_buf = TRealAllocator::alloc(new_size, alignment);
        memcpy(new_buf, buf, old_size);
        return new_buf;
    }

    void free(void * buf, unsigned long size)
    {
        if (size >= REAL_ALLOCATION_TRESHOLD)
            TRealAllocator::free(buf, size);
    }

protected:
    static constexpr unsigned long getStackThreshold()
    {
        return 0;
    }
};


template <unsigned long alignment, unsigned long REAL_ALLOCATION_TRESHOLD = 4096>
using MixedAlignedArenaAllocator = MixedArenaAllocator<REAL_ALLOCATION_TRESHOLD, Allocator<false>, AlignedArenaAllocator<alignment>, alignment>;


template <unsigned long N = 64, typename Base = ArenaAllocator>
class ArenaAllocatorWithStackMemory : public Base
{
    char stack_memory[N];

public:

    void * alloc(unsigned long size, Arena * arena)
    {
        return (size > N) ? Base::alloc(size, arena) : stack_memory;
    }

    void * realloc(void * buf, unsigned long old_size, unsigned long new_size, Arena * arena)
    {
        /// Was in stack_memory, will remain there.
        if (new_size <= N)
            return buf;

        /// Already was big enough to not fit in stack_memory.
        if (old_size > N)
            return Base::realloc(buf, old_size, new_size, arena);

        /// Was in stack memory, but now will not fit there.
        void * new_buf = Base::alloc(new_size, arena);
        memcpy(new_buf, buf, old_size);
        return new_buf;
    }

    void free(void * /*buf*/, unsigned long /*size*/) {}

protected:
    static constexpr unsigned long getStackThreshold()
    {
        return N;
    }
};

}
