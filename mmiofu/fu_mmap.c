// Offload a computation to a MMI/O Functional Unit
// Communication page is 0xFFFEF000 in physical memory
//
// protocol I'm using for this test:
// 0xFFFEF000: double A (arg)
//             double B (arg)
//             double C (result)

#include "fu_mmap.h"

void _create_io_map()
{
  int fd = open("/dev/mem", O_RDWR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  // confirm that IO_PAGE is actually on a page boundary
  assert((IO_PAGE & (sysconf(_SC_PAGE_SIZE) - 1)) == 0x0);

  void * addr =
    mmap(NULL, IO_PAGE_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, IO_PAGE);
  if (addr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  
  close(fd);
  _io_map = addr;
  // fprintf(stderr, "Success mapping: %p => %#x\n", _io_map, IO_PAGE);
}

// closed automatically on exit
