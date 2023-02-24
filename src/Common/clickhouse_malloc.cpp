#include <Common/memory.h>
#include <cstdlib>


/** These functions can be substituted instead of regular ones when memory tracking is needed.
  */

extern "C" void * clickhouse_malloc(unsigned long size)
{
    void * res = malloc(size);
    if (res)
        Memory::trackMemory(size);
    return res;
}

extern "C" void * clickhouse_calloc(unsigned long number_of_members, unsigned long size)
{
    void * res = calloc(number_of_members, size);
    if (res)
        Memory::trackMemory(number_of_members * size);
    return res;
}

extern "C" void * clickhouse_realloc(void * ptr, unsigned long size)
{
    if (ptr)
        Memory::untrackMemory(ptr);
    void * res = realloc(ptr, size);
    if (res)
        Memory::trackMemory(size);
    return res;
}

extern "C" void * clickhouse_reallocarray(void * ptr, unsigned long number_of_members, unsigned long size)
{
    unsigned long real_size = 0;
    if (__builtin_mul_overflow(number_of_members, size, &real_size))
        return nullptr;

    return clickhouse_realloc(ptr, real_size);
}

extern "C" void clickhouse_free(void * ptr)
{
    Memory::untrackMemory(ptr);
    free(ptr);
}

extern "C" int clickhouse_posix_memalign(void ** memptr, unsigned long alignment, unsigned long size)
{
    int res = posix_memalign(memptr, alignment, size);
    if (res == 0)
        Memory::trackMemory(size);
    return res;
}
