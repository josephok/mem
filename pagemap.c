#include <assert.h>
#include "mem.h"

int pagemap_get_entry(PagemapEntry entry[], size_t entry_size, int pagemap_fd, uintptr_t vaddr)
{
    uint64_t data[entry_size];
    uintptr_t vpn = vaddr / PAGE_SIZE;
    ssize_t nread;

    if ((nread = pread(pagemap_fd, data, sizeof(data), vpn * sizeof(uint64_t))) < 0)
        return -1;
    for (size_t i = 0; i < nread / sizeof(uint64_t); i++) {
        entry[i].pfn = data[i] & (((uint64_t)1 << 55) - 1);
        entry[i].soft_dirty = (data[i] >> 55) & 1;
        entry[i].file_page = (data[i] >> 61) & 1;
        entry[i].swapped = (data[i] >> 62) & 1;
        entry[i].present = (data[i] >> 63) & 1;
    }
    return nread / sizeof(uint64_t);
}
