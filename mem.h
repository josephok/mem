#ifndef _MEM_H
#define _MEM_H

#include <stdint.h>
#include <unistd.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define PAGE_ENTRY_SIZE 8
#define PAGE_TO_KB(npages) (npages) * PAGE_SIZE/1024
#define PRINT_USGAE(status) do { \
    fprintf(stderr, "Usage: %s [-a address range(eg. ffffa10d1000-ffffa10d3000)] pid\n", argv[0]); \
    exit(status); \
    } while(0)

#define PID_INVALID() do { \
        fprintf(stderr, "pid no exits\n"); \
        exit(EXIT_FAILURE); \
    } while(0)

typedef struct {
    uint64_t pfn : 55;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

void get_kpageflags(char *buf, uint64_t flags);
int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr);
void print_mem_usage(pid_t pid);
uint64_t read_u64(int fd, size_t offset);
void print_flags(uintptr_t addr_s, uintptr_t addr_e, pid_t pid);

#endif
