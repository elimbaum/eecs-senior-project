// mostly from the mmap man page

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main(int argc, char * argv[])
{
  char * addr;
  int fd;
  struct stat sb;
  off_t offset, pa_offset;
  size_t length;
  ssize_t s;

  if (argc != 3) {
    fprintf(stderr, "%s offset length\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  fd = open("/dev/mem", O_RDWR);
  if (fd == -1)
    handle_error("open");

  offset = strtol(argv[1], NULL, 0);
  // offset must be page aligned
  pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

  if (offset != pa_offset)
    fprintf(stderr, "warning: offsets must be page aligned (was %lx, now %lx)\n", offset, pa_offset);

  length = strtol(argv[2], NULL, 0);

  addr = mmap(NULL, length + offset - pa_offset, PROT_READ | PROT_WRITE,
              MAP_SHARED, fd, pa_offset);
  if (addr == MAP_FAILED)
    handle_error("mmap");

  // READ to stdout
  s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
  if (s != length) {
    if (s == - 1)
      handle_error("write");

    fprintf(stderr, "partial write\n");
    exit(EXIT_FAILURE);
  }

  munmap(addr, length + offset - pa_offset);
  close(fd);

  exit(EXIT_SUCCESS);
}
