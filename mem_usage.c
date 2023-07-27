#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include "mem.h"

void print_mem_usage(pid_t pid)
{
    char pagemap_file[BUFSIZ];
    char maps_file[BUFSIZ];

    snprintf(maps_file, sizeof(maps_file), "/proc/%d/maps", pid);
    snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%d/pagemap", pid);
    FILE *maps_fp = fopen(maps_file, "r");
    if (!maps_fp)
        err(EXIT_FAILURE, "open %s failed", maps_file);

    int pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0)
        err(EXIT_FAILURE, "open %s failed", pagemap_file);

    char buf[BUFSIZ];
    uintptr_t vaddr_start, vaddr_end;
    PagemapEntry entry[READ_COUNT];

    size_t vsz = 0;
    size_t rss = 0;
    size_t pss = 0;
    size_t uss = 0;
    int kpagecount_fd = open("/proc/kpagecount", O_RDONLY);
    if (kpagecount_fd < 0)
        err(EXIT_FAILURE, "open /proc/kpagecount failed");

    while (fgets(buf, BUFSIZ, maps_fp)) {
        buf[strcspn(buf, "\n")] = ' ';
        printf("%s", buf);
        char *endptr;
        vaddr_start = strtoull(buf, &endptr, 16);
        vaddr_end = strtoull(endptr+1, &endptr, 16);

        printf("VSZ:%luKB ", (vaddr_end - vaddr_start)/1024);
        vsz += (vaddr_end - vaddr_start)/1024;
        size_t npages = (vaddr_end - vaddr_start)/PAGE_SIZE;
        size_t line_rss = 0;
        size_t line_uss = 0;
        double line_pss = 0.0;
        int nread;
        int nleft = npages;
        for (size_t i = 0; i < npages; i += READ_COUNT) {
            uintptr_t vaddr = vaddr_start + i*PAGE_SIZE;
            if ((nread = pagemap_get_entry(entry, MIN(nleft, READ_COUNT), pagemap_fd, vaddr)) < 0)
                err(EXIT_FAILURE, "read %s failed", pagemap_file);

            nleft -= nread;

            for (size_t j = 0; j < nread; j++) {
                if (entry[j].present) {
                    line_rss++;
                    uint64_t count = read_u64(kpagecount_fd, entry[j].pfn * PAGE_ENTRY_SIZE);
                    if (count == 1)
                        line_uss++;
                    else if (count > 1) {
                        line_pss += 1.0/count;
                    }
                }
            }
        }
        printf("RSS:%luKB ", PAGE_TO_KB(line_rss));
        printf("PSS:%luKB ", (size_t)(PAGE_TO_KB(line_uss + line_pss)));
        printf("USS:%luKB\n", PAGE_TO_KB(line_uss));
        rss += PAGE_TO_KB(line_rss);
        pss += PAGE_TO_KB(line_uss + line_pss);
        uss += PAGE_TO_KB(line_uss);
    }
    printf("VSZ:%luKB\t RSS:%luKB\t PSS:%luKB\t USS:%luKB\n", vsz, rss, pss, uss);
    close(pagemap_fd);
    close(kpagecount_fd);
    fclose(maps_fp);
}
