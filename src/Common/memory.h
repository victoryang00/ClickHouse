#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <new>
#include <base/defines.h>

#include <Common/Concepts.h>
#include <Common/CurrentMemoryTracker.h>
#include <Common/config.h>

#if USE_JEMALLOC
#    include <jemalloc/jemalloc.h>
#endif

#if !USE_JEMALLOC
#    include <cstdlib>
#endif

namespace Memory
{

inline ALWAYS_INLINE unsigned long alignToSizeT(std::align_val_t align) noexcept
{
    return static_cast<unsigned long>(align);
}

template <std::same_as<std::align_val_t>... TAlign>
requires DB::OptionalArgument<TAlign...>
inline ALWAYS_INLINE void * newImpl(unsigned long size, TAlign... align)
{
    void * ptr = nullptr;
    if constexpr (sizeof...(TAlign) == 1)
        ptr = aligned_alloc(alignToSizeT(align...), size);
    else
        ptr = malloc(size);

    if (likely(ptr != nullptr))
        return ptr;

    /// @note no std::get_new_handler logic implemented
    throw std::bad_alloc{};
}

inline ALWAYS_INLINE void * newNoExept(unsigned long size) noexcept
{
    return malloc(size);
}

inline ALWAYS_INLINE void * newNoExept(unsigned long size, std::align_val_t align) noexcept
{
    return aligned_alloc(static_cast<unsigned long>(align), size);
}

inline ALWAYS_INLINE void deleteImpl(void * ptr) noexcept
{
    free(ptr);
}

#if USE_JEMALLOC

template <std::same_as<std::align_val_t>... TAlign>
requires DB::OptionalArgument<TAlign...>
inline ALWAYS_INLINE void deleteSized(void * ptr, unsigned long size, TAlign... align) noexcept
{
    if (unlikely(ptr == nullptr))
        return;

    if constexpr (sizeof...(TAlign) == 1)
        sdallocx(ptr, size, MALLOCX_ALIGN(alignToSizeT(align...)));
    else
        sdallocx(ptr, size, 0);
}

#else

template <std::same_as<std::align_val_t>... TAlign>
requires DB::OptionalArgument<TAlign...>
inline ALWAYS_INLINE void deleteSized(void * ptr, unsigned long size [[maybe_unused]], TAlign... /* align */) noexcept
{
    free(ptr);
}

#endif

#if defined(OS_LINUX)
#    include <malloc.h>
#elif defined(OS_DARWIN)
#    include <malloc/malloc.h>
#endif

template <std::same_as<std::align_val_t>... TAlign>
requires DB::OptionalArgument<TAlign...>
inline ALWAYS_INLINE unsigned long getActualAllocationSize(unsigned long size, TAlign... align [[maybe_unused]])
{
    unsigned long actual_size = size;

#if USE_JEMALLOC
    /// The nallocx() function allocates no memory, but it performs the same size computation as the mallocx() function
    /// @note je_mallocx() != je_malloc(). It's expected they don't differ much in allocation logic.
    if (likely(size != 0))
    {
        if constexpr (sizeof...(TAlign) == 1)
            actual_size = nallocx(size, MALLOCX_ALIGN(alignToSizeT(align...)));
        else
            actual_size = nallocx(size, 0);
    }
#endif

    return actual_size;
}

template <std::same_as<std::align_val_t>... TAlign>
requires DB::OptionalArgument<TAlign...>
inline ALWAYS_INLINE void trackMemory(unsigned long size, TAlign... align)
{
    unsigned long actual_size = getActualAllocationSize(size, align...);
    CurrentMemoryTracker::allocNoThrow(actual_size);
}

template <std::same_as<std::align_val_t>... TAlign>
requires DB::OptionalArgument<TAlign...>
inline ALWAYS_INLINE void untrackMemory(void * ptr [[maybe_unused]], unsigned long size [[maybe_unused]] = 0, TAlign... align [[maybe_unused]]) noexcept
{
    try
    {
#if USE_JEMALLOC

        /// @note It's also possible to use je_malloc_usable_size() here.
        if (likely(ptr != nullptr))
        {
            if constexpr (sizeof...(TAlign) == 1)
                CurrentMemoryTracker::free(sallocx(ptr, MALLOCX_ALIGN(alignToSizeT(align...))));
            else
                CurrentMemoryTracker::free(sallocx(ptr, 0));
        }
#else
        if (size)
            CurrentMemoryTracker::free(size);
#    if defined(_GNU_SOURCE) && !defined(__EMSCRIPTEN__)
        /// It's innaccurate resource free for sanitizers. malloc_usable_size() result is greater or equal to allocated size.
        else
            CurrentMemoryTracker::free(malloc_usable_size(ptr));
#    endif
#endif
    }
    catch (...)
    {
    }
}

}

#pragma GCC diagnostic pop
