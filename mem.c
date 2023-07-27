#include <err.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h> /* open */
#include <stdint.h> /* uint64_t  */
#include <stdio.h> /* printf */
#include <stdlib.h> /* size_t */
#include <unistd.h> /* pread, sysconf */
#include "mem.h"

void print_cmdline(pid_t pid)
{
    char buf[BUFSIZ];

    snprintf(buf, BUFSIZ, "/proc/%d/cmdline", pid);
    FILE *fp = fopen(buf, "r");
    if (!fp)
        PID_INVALID();

    if (fgets(buf, BUFSIZ, fp) == NULL)
        err(EXIT_FAILURE, "read status failed");
    printf("%d: %s\n", pid, buf);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        PRINT_USGAE(EXIT_FAILURE);
    }

    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        PRINT_USGAE(EXIT_SUCCESS);
    }
    int opt;
    uintptr_t addr_s = 0, addr_e = 0;
    char *endptr;
    while ((opt = getopt(argc, argv, "a:")) != -1) {
        switch (opt) {
            case 'a':
                addr_s = strtoull(optarg, &endptr, 16);
                addr_e = strtoull(endptr+1, &endptr, 16);
                if (addr_e <= addr_s)
                    addr_e = addr_s + PAGE_SIZE;

                if (addr_s % PAGE_SIZE != 0 || addr_e % PAGE_SIZE != 0) {
                    fprintf(stderr, "address must be 0x%lx bytes aligned\n", PAGE_SIZE);
                    exit(EXIT_FAILURE);
                }
                break;
            default: /* '?' */
                PRINT_USGAE(EXIT_FAILURE);
        }
    }

    if (optind < 1) {
        PRINT_USGAE(EXIT_FAILURE);
    }

    pid_t pid = strtoull(argv[optind], NULL, 0);
    print_cmdline(pid);

    if (addr_s && addr_e)
        print_flags(addr_s, addr_e, pid);
    else
        print_mem_usage(pid);
}

