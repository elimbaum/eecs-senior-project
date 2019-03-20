// scan the whole address range to determine memory layout

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define UPPER_LIMIT 0x10000000L
#define LENGTH 64

int main(int argc, char * argv[])
{
  char * addr;
  int fd;
  off_t offset, pa_offset;
  size_t length;
  ssize_t s;

  enum MemState { UNK, OPEN, CLOSED } mem_state;

  fd = open("/dev/mem", O_RDWR);
  if (fd == -1)
    handle_error("open");

  long step = sysconf(_SC_PAGE_SIZE);
  for(off_t offset = 0; offset <= UPPER_LIMIT; offset += step) {
    addr = mmap(NULL, LENGTH + offset, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);

    if (addr == MAP_FAILED) {
      if (mem_state != CLOSED) {
        printf("CLOSED @ %lx\n", offset);
        mem_state = CLOSED;
      }
      // handle_error("mmap");
    } else if (mem_state != OPEN) {
      printf("OPEN   @ 0x%lX\n", offset);
      mem_state = OPEN;
    }
    //printf("%lX\n", offset);

    munmap(addr, LENGTH + offset);
  }

  close(fd);
  exit(EXIT_SUCCESS);
}
