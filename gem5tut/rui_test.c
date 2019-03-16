#include <stdio.h>    
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
	  off_t offset = 0x3500;
	  size_t len = 8; // sizeof(unsigned);

    if (argc == 2) {
      offset = strtol(argv[1], NULL, 0);
    }
    
    // Truncate offset to a multiple of the page size, or mmap will fail.
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t page_offset = offset - page_base;
    printf("page size: %lX, page base: %lX, offset: %lX\n", pagesize, page_base, (page_offset + len));

    int fd = open("/dev/mem", O_RDWR);
    assert(fd != -1);
    unsigned char *mem = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, page_base);
    if (mem == MAP_FAILED) {
        perror("Can't map memory");
        return -1;
    }

    size_t i, s;
    // for (i = 0; i < len; ++i)
    //    printf("%02x ", (int)mem[page_offset + i]);

    for (s = page_offset;; ++s)
      if ((int)mem[s] != 0)
        break;

    printf("Found non-zero @ %lx\n", s);

    for (i = 0; i < len; ++i)
      printf("%02x ", (int)mem[s + i]);

    printf("\n");
    return 0;
}
