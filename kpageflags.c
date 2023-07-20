#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <error.h>
#include <errno.h>
#include "mem.h"

void print_flags(uintptr_t vaddr_start, uintptr_t vaddr_end, pid_t pid)
{
    size_t npages = (vaddr_end - vaddr_start)/PAGE_SIZE;
    if (npages < 1)
        error(EXIT_FAILURE, EINVAL, "invalid address");
    int kpageflags_fd = open("/proc/kpageflags", O_RDONLY);
    if (kpageflags_fd < 0)
        err(EXIT_FAILURE, "open /proc/kpageflags failed");

    char pagemap_file[BUFSIZ];

    snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%d/pagemap", pid);
    int pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0)
        err(EXIT_FAILURE, "open %s failed", pagemap_file);

    PagemapEntry entry;
    uintptr_t phy_addr;
    for (size_t i = 0; i < npages; i++) {
        uintptr_t vaddr = vaddr_start + i*PAGE_SIZE;
        if (pagemap_get_entry(&entry, pagemap_fd, vaddr))
            err(EXIT_FAILURE, "read %s failed", pagemap_file);
        phy_addr = (entry.pfn * PAGE_SIZE) + (vaddr % PAGE_SIZE);
        uint64_t flags = read_u64(kpageflags_fd, entry.pfn * PAGE_ENTRY_SIZE);
        char buf[BUFSIZ] = {0};
        get_kpageflags(buf, flags);

        printf("virtual addr: 0x%jx, physical addr: 0x%jx, flags: %s\n", vaddr, phy_addr, buf);
    }
    close(kpageflags_fd);
    close(pagemap_fd);
}

void get_kpageflags(char *buf, uint64_t flags)
{
    if (flags & 1)
        strcat(buf, "LOCKED ");
    if (flags >> 1 & 1)
        strcat(buf, "ERROR ");
    if (flags >> 2 & 1)
        strcat(buf, "REFERENCED ");
    if (flags >> 3 & 1)
        strcat(buf, "UPTODATE ");
    if (flags >> 4 & 1)
        strcat(buf, "DIRTY ");
    if (flags >> 5 & 1)
        strcat(buf, "LRU ");
    if (flags >> 6 & 1)
        strcat(buf, "ACTIVE ");
    if (flags >> 7 & 1)
        strcat(buf, "SLAB ");
    if (flags >> 8 & 1)
        strcat(buf, "WRITEBACK ");
    if (flags >> 9 & 1)
        strcat(buf, "RECLAIM ");
    if (flags >> 10 & 1)
        strcat(buf, "BUDDY ");
    if (flags >> 11 & 1)
        strcat(buf, "MMAP ");
    if (flags >> 12 & 1)
        strcat(buf, "ANON ");
    if (flags >> 13 & 1)
        strcat(buf, "SWAPCACHE ");
    if (flags >> 14 & 1)
        strcat(buf, "SWAPBACKED ") ;
    if (flags >> 15 & 1)
        strcat(buf, "COMPOUND_HEAD ") ;
    if (flags >> 16 & 1)
        strcat(buf, "COMPOUND_TAIL ");
    if (flags >> 17 & 1)
        strcat(buf, "HUGE ");
    if (flags >> 18 & 1)
        strcat(buf, "UNEVICTABLE ");
    if (flags >> 19 & 1)
        strcat(buf, "HWPOISON ") ;
    if (flags >> 20 & 1)
        strcat(buf, "NOPAGE ");
    if (flags >> 21 & 1)
        strcat(buf, "KSM ");
    if (flags >> 22 & 1)
        strcat(buf, "THP ");
    if (flags >> 23 & 1)
        strcat(buf, "OFFLINE ") ;
    if (flags >> 24 & 1)
        strcat(buf, "ZERO_PAGE ");
    if (flags >> 25 & 1)
        strcat(buf, "IDLE ");
    if (flags >> 26 & 1)
        strcat(buf, "PGTABLE ");
}
