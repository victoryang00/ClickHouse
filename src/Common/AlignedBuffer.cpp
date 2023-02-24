#include <Common/AlignedBuffer.h>

#include <Common/Exception.h>
#include <Common/formatReadable.h>


namespace DB
{

namespace ErrorCodes
{
    extern const int CANNOT_ALLOCATE_MEMORY;
}


void AlignedBuffer::alloc(unsigned long size, unsigned long alignment)
{
    void * new_buf;
    int res = ::posix_memalign(&new_buf, std::max(alignment, sizeof(void*)), size);
    if (0 != res)
        throwFromErrno(fmt::format("Cannot allocate memory (posix_memalign), size: {}, alignment: {}.",
            ReadableSize(size), ReadableSize(alignment)),
            ErrorCodes::CANNOT_ALLOCATE_MEMORY, res);
    buf = new_buf;
}

void AlignedBuffer::dealloc()
{
    if (buf)
        ::free(buf);
}

void AlignedBuffer::reset(unsigned long size, unsigned long alignment)
{
    dealloc();
    alloc(size, alignment);
}

AlignedBuffer::AlignedBuffer(unsigned long size, unsigned long alignment)
{
    alloc(size, alignment);
}

AlignedBuffer::~AlignedBuffer()
{
    dealloc();
}

}
