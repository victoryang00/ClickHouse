#pragma once

#include <cstdlib>
#include <utility>
#include <boost/noncopyable.hpp>


namespace DB
{

/** Aligned piece of memory.
  * It can only be allocated and destroyed.
  * MemoryTracker is not used. AlignedBuffer is intended for small pieces of memory.
  */
class AlignedBuffer : private boost::noncopyable
{
private:
    void * buf = nullptr;

    void alloc(unsigned long size, unsigned long alignment);
    void dealloc();

public:
    AlignedBuffer() = default;
    AlignedBuffer(unsigned long size, unsigned long alignment);
    AlignedBuffer(AlignedBuffer && old) noexcept { std::swap(buf, old.buf); }
    ~AlignedBuffer();

    void reset(unsigned long size, unsigned long alignment);

    char * data() { return static_cast<char *>(buf); }
    const char * data() const { return static_cast<const char *>(buf); }
};

}

