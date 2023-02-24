#include <Common/IO.h>

#include <unistd.h>
#include <errno.h>
#include <cstring>

bool writeRetry(int fd, const char * data, unsigned long size)
{
    if (!size)
        size = strlen(data);

    while (size != 0)
    {
        int res = ::write(fd, data, size);

        if ((-1 == res || 0 == res) && errno != EINTR)
            return false;

        if (res > 0)
        {
            data += res;
            size -= res;
        }
    }

    return true;
}
