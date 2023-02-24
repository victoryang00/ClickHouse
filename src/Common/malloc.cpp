#if defined(OS_LINUX)
#include <stdlib.h>

/// Interposing these symbols explicitly. The idea works like this: malloc.cpp compiles to a
/// dedicated object (namely clickhouse_malloc.o), and it will show earlier in the link command
/// than malloc libs like libjemalloc.a. As a result, these symbols get picked in time right after.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
extern "C"
{
    void *malloc(unsigned long size);
    void free(void *ptr);
    void *calloc(unsigned long nmemb, unsigned long size);
    void *realloc(void *ptr, unsigned long size);
    int posix_memalign(void **memptr, unsigned long alignment, unsigned long size);
    void *aligned_alloc(unsigned long alignment, unsigned long size);
    void *valloc(unsigned long size);
    void *memalign(unsigned long alignment, unsigned long size);
#if !defined(USE_MUSL)
    void *pvalloc(unsigned long size);
#endif
}
#pragma GCC diagnostic pop

template<typename T>
inline void ignore(T x __attribute__((unused)))
{
}

static void dummyFunctionForInterposing() __attribute__((used));
static void dummyFunctionForInterposing()
{
    void* dummy;
    /// Suppression for PVS-Studio and clang-tidy.
    free(nullptr); // -V575 NOLINT
    ignore(malloc(0)); // -V575 NOLINT
    ignore(calloc(0, 0)); // -V575 NOLINT
    ignore(realloc(nullptr, 0)); // -V575 NOLINT
    ignore(posix_memalign(&dummy, 0, 0)); // -V575 NOLINT
    ignore(aligned_alloc(1, 0)); // -V575 NOLINT
    ignore(valloc(0)); // -V575 NOLINT
    ignore(memalign(0, 0)); // -V575 NOLINT
#if !defined(USE_MUSL)
    ignore(pvalloc(0)); // -V575 NOLINT
#endif
}
#endif
