#include "mem.h"

uint64_t read_u64(int fd, off_t offset)
{
    uint64_t data;
    if (pread(fd, &data, sizeof(data), offset) < 0)
        return -1;
    return data;
}

ssize_t read_u64_n(int fd, off_t offset, uint64_t data[], size_t n)
{

    ssize_t nread;
    if ((nread = pread(fd, data, sizeof(uint64_t) * n, offset)) < 0)
        return -1;

    return nread / sizeof(uint64_t);
}
