#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <type_traits>
#include <base/defines.h>


/** For zero argument, result is zero.
  * For arguments with most significand bit set, result is n.
  * For other arguments, returns value, rounded up to power of two.
  */
inline unsigned long long roundUpToPowerOfTwoOrZero(unsigned long long n)
{
    // if MSB is set, return n, to avoid return zero
    if (unlikely(n >= 0x8000000000000000ULL))
        return n;

    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    ++n;

    return n;
}


template <typename T>
inline unsigned long getLeadingZeroBitsUnsafe(T x)
{
    assert(x != 0);

    if constexpr (sizeof(T) <= sizeof(unsigned int))
    {
        return __builtin_clz(x);
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long int)) /// NOLINT
    {
        return __builtin_clzl(x);
    }
    else
    {
        return __builtin_clzll(x);
    }
}


template <typename T>
inline unsigned long getLeadingZeroBits(T x)
{
    if (!x)
        return sizeof(x) * 8;

    return getLeadingZeroBitsUnsafe(x);
}

/** Returns log2 of number, rounded down.
  * Compiles to single 'bsr' instruction on x86.
  * For zero argument, result is unspecified.
  */
template <typename T>
inline uint32_t bitScanReverse(T x)
{
    return (std::max<unsigned long>(sizeof(T), sizeof(unsigned int))) * 8 - 1 - getLeadingZeroBitsUnsafe(x);
}

// Unsafe since __builtin_ctz()-family explicitly state that result is undefined on x == 0
template <typename T>
inline unsigned long getTrailingZeroBitsUnsafe(T x)
{
    assert(x != 0);

    if constexpr (sizeof(T) <= sizeof(unsigned int))
    {
        return __builtin_ctz(x);
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long long)) /// NOLINT
    {
        return __builtin_ctzl(x);
    }
    else
    {
        return __builtin_ctzll(x);
    }
}

template <typename T>
inline unsigned long getTrailingZeroBits(T x)
{
    if (!x)
        return sizeof(x) * 8;

    return getTrailingZeroBitsUnsafe(x);
}

/** Returns a mask that has '1' for `bits` LSB set:
  * maskLowBits<UInt8>(3) => 00000111
  */
template <typename T>
inline T maskLowBits(unsigned char bits)
{
    if (bits == 0)
    {
        return 0;
    }

    T result = static_cast<T>(~T{0});
    if (bits < sizeof(T) * 8)
    {
        result = static_cast<T>(result >> (sizeof(T) * 8 - bits));
    }

    return result;
}
