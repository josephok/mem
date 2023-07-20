#include "mem.h"

uint64_t read_u64(int fd, size_t offset)
{
    uint64_t data;
    if (pread(fd, &data, sizeof(data), offset) < 0)
        return -1;
    return data;
}
